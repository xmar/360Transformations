import multiprocessing as mp
import os
import numpy as np
import subprocess as sub
import shutil
import re
import math
import queue
from timeit import default_timer as timer
import sys, traceback

import SearchTools
import GenerateVideo
import LayoutGenerators
import FormatResults
from .Results import Results
import MultiProcess.Video as Video

def WorkerManager(GenericWorker, shared_job_q, shared_result_q, exit_event):
    """Run a unique process that will run the SpecializedWorker"""
    proc = mp.Process(target=GenericWorker.Run,
            args=(shared_job_q, shared_result_q))

    proc.start()

    while True:
        proc.join(5)
        try:
            if not proc.is_alive():
                break
            elif exit_event.is_set():
                proc.terminate()
                proc.join(5)
        except:
            proc.terminate()
            proc.join(5)



class GenericWorker(object):
    def __init__(self, trans, config, outDir, hostname):
        self.trans = trans
        self.config = config
        self.outDir = outDir
        self.hostname = hostname

    def Run(self, shared_job_q, shared_result_q):
        self.outputVideoDir = '{}/InputVideos'.format(self.outDir)
        try:
            while True:
                self.job = None
                self.job = shared_job_q.get_nowait()
                self.n = self.job.nbFrames
                self.video = self.job.inputVideo
                if not os.path.exists(self.outputVideoDir):
                    os.mkdir(self.outputVideoDir)
                self.video.UpdateVideoDir(self.outputVideoDir)
                if not os.path.exists(self.video.realPath) or Video.Video(self.video.realPath) != self.video:
                    Video.VideoReceiver(self.video, self.hostname)
                self.inputVideo = self.video.realPath
                self.maxIteration = self.job.nbDicothomicInteration
                self.reuseVideo = self.job.reuseVideo
                self.outputResolution = self.job.res.split('x')
                self.outputResolution = (int(self.outputResolution[0]), int(self.outputResolution[1]))
                self.k = self.job.nbTest
                self.nbQec = self.job.nbQec
                self.averageGoalSize = (0,0)
                self.bitrateGoal = int(self.job.bitrateGoal)
                self.distStep = self.job.distStep
                self.outputDir = '{}/{}'.format(self.outDir, self.job.ToDirName())
                if not os.path.exists(self.outputDir):
                    os.mkdir(self.outputDir)
    
                self.comment = self.job.ToComment()
                print('Start job {}'.format(self.comment))
                startTime = timer() 
                workDone = False
                try:
                    (workDone, result) = self.SpecializedWorker()
                except KeyboardInterrupt:
                    raise
                except Exception as e:
                    print(e)
                    raise e
                finally:
                    endTime = timer()
                    print('Elapsed time = {}s'.format(endTime-startTime))
                    if workDone:
                        try:
                            result.SetElapsedTime(endTime-startTime)
                            shared_result_q.put(result)
                            print('Result sent to the server')
                        except ConnectionResetError:
                            print('Connection with the server lost...')
                            return
                    else:
                        try:
                            shared_job_q.put(self.job)
                            print('Job sent back to the server')
                        except ConnectionResetError:
                            print('Connection with the server lost...')
                            return


        except KeyboardInterrupt:
            print('Test interrupted')
            raise
        except queue.Empty:
            return

    def SpecializedWorker(self):
        return self.RunningTests()

    def RunningTests(self):
        raise NotImplementedError


class FixedBitrateAndFixedDistances(GenericWorker):
    def __init__(self, trans, config, outDir, hostname):
        super().__init__(trans, config, outDir, hostname)
        #super().RunningTests = self.__RunningTests__

    def RunningTests(self):
        self.__GenerateBaseVideos__()
        self.__GenerateAverageVideos__()
        return self.__RunTest__()

    def __GenerateBaseVideos__(self):
        '''Generate the base video for the different layouts and differents QECs'''
        #First we re-encode the original Equirectangular video for fair comparaison later
        outEquiNameStorage = '{}/equirectangular_storage.dat'.format(self.outputDir)
        outEquiNameVideo = '{}/equirectangular.mkv'.format(self.outputDir)
        outEquiId = '{}/equirectangular'.format(self.outputDir)
        GenerateVideo.GenerateVideoAndStore(self.config, self.trans, [(LayoutGenerators.EquirectangularLayout('Equirectangular'), None)], 24, self.n,
                self.inputVideo, outEquiId, self.bitrateGoal)
        
        #We get the resolution of the video
        ffmpegProcess = sub.Popen(['ffmpeg', '-i', outEquiNameVideo], stderr=sub.PIPE)
        regex = re.compile('.*\s(\d+)x(\d+)\s.*')
        for line in iter(ffmpegProcess.stderr.readline, b''):
            m = regex.match(line.decode('utf-8'))
            if m is not None:
                self.refWidth = int(m.group(1))
                self.refHeight = int(m.group(2))
        
        #for each QEC we compute the EquirectangularTiled layout associated + the other layout with a fixed bitrate
        for qec in LayoutGenerators.QEC.TestQecGenerator(self.nbQec):
            qecId = qec.GetStrId()
            print('Start computation for QEC({})'.format(qecId))
            outEquiTiledNameStorage = '{}/equirectangularTiled{}_storage.dat'.format(self.outputDir,qecId)
            outEquiTiledNameVideo = '{}/equirectangularTiled{}.mkv'.format(self.outputDir,qecId)
            outEquiTiledId = '{}/equirectangularTiled{}'.format(self.outputDir,qecId)
            GenerateVideo.GenerateVideoAndStore(self.config, self.trans,
                    [(LayoutGenerators.EquirectangularLayout('Equirectangular'), None),
                     (LayoutGenerators.EquirectangularTiledLayout('EquirectangularTiled{}'.format(qecId),qec, self.refWidth, self.refHeight), None)],
                    24, self.n,  self.inputVideo, outEquiTiledId, self.bitrateGoal)
        
            goalSize = os.stat(outEquiTiledNameVideo).st_size
            self.averageGoalSize = (self.averageGoalSize[0] + goalSize, self.averageGoalSize[1]+1)
        
            for (lName, lGenerator) in [('CubeMap{}'.format(qecId), (lambda n,ypr: LayoutGenerators.CubeMapLayout(n, ypr, self.refWidth, self.refHeight))),\
                    ('CubeMapCompact{}'.format(qecId), (lambda n,ypr: LayoutGenerators.CubeMapLayoutCompact(n, ypr, self.refWidth, self.refHeight))),\
                    ('Pyramidal{}'.format(qecId),  (lambda n,ypr: LayoutGenerators.PyramidLayout(n,ypr,2.5, self.refWidth, self.refHeight))),\
                    ('RhombicDodeca{}'.format(qecId), (lambda n,ypr: LayoutGenerators.RhombicDodecaLayout(n, ypr, self.refWidth, self.refHeight)))]:
                layout = lGenerator(lName, qec.GetEulerAngles())
                outLayoutId = '{}/{}'.format(self.outputDir,lName)
                #SearchTools.DichotomousSearch(trans, config, n, inputVideo, outLayoutId, goalSize, layout, maxIteration)
                GenerateVideo.GenerateVideoAndStore(self.config, self.trans,
                        [(LayoutGenerators.EquirectangularLayout('Equirectangular'), None),(layout, 0.5)], 24, self.n, self.inputVideo, outLayoutId, self.bitrateGoal)

    def __GenerateAverageVideos__(self):
        #Compute the average equirectangularTiled video
        averageGoalSize = self.averageGoalSize[0]/self.averageGoalSize[1]
        averageNameStorage = '{}/AverageEquiTiled_storage.dat'.format(self.outputDir)
        self.averageNameVideo = '{}/AverageEquiTiled.mkv'.format(self.outputDir)
        skip = False
        if os.path.isfile(averageNameStorage) and os.path.isfile(self.averageNameVideo):
            self.lsAverage = LayoutGenerators.LayoutStorage.Load(averageNameStorage)
            if self.n == self.lsAverage.nbFrames:
                skip = True
        if not skip:
            outLayoutId = '{}/AverageEquiTiled'.format(self.outputDir)
            layoutAverage = LayoutGenerators.EquirectangularTiledLayout('AverageEquiTiled', None, self.refWidth, self.refHeight)
            SearchTools.DichotomousSearch(self.trans, self.config, self.n, self.inputVideo, outLayoutId, averageGoalSize, layoutAverage, self.maxIteration)
            self.lsAverage = LayoutGenerators.LayoutStorage.Load(averageNameStorage)

    def __RunFlatFixedViewTest__(self, point, currentQec):
        (cy, cp) = point
        center = (cy, cp, 0)
        closestQec = LayoutGenerators.QEC.GetClosestQecFromTestQec(cy, cp, self.nbQec)
        (i,j) = currentQec.GetTileCoordinate()
        qecId = currentQec.GetStrId()
        (y,p,r) = currentQec.GetEulerAngles()
        goodPoint = (currentQec == closestQec)
        #First we check if the output folder for this QEC exist:
        outputDirQEC = '{}/QEC{}'.format(self.outputDir, qecId)
        if not os.path.isdir(outputDirQEC):
            os.makedirs(outputDirQEC)
        eqL = LayoutGenerators.EquirectangularLayout('Equirectangular')
        if self.reuseVideo:
            inputVideos = ['{}/equirectangular.mkv'.format(self.outputDir), '{}/equirectangularTiled{}.mkv'.format(self.outputDir,qecId), self.averageNameVideo]
            layoutsToTest = [[(eqL, None)], [(LayoutGenerators.EquirectangularTiledLayout('EquirectangularTiled{}'.format(qecId), currentQec, self.refWidth, self.refHeight), None)], \
                    [(self.lsAverage.layout, self.lsAverage.a)]]
        else:
            inputVideos = [self.inputVideo, self.inputVideo, self.inputVideo]
            layoutsToTest = [[(eqL, None)], [(eqL, None),(LayoutGenerators.EquirectangularTiledLayout('EquirectangularTiled{}'.format(qecId), currentQec, self.refWidth, self.refHeight), None)], \
                    [(eqL, None),(self.lsAverage.layout, self.lsAverage.a)]]
        for layoutId in ['CubeMap', \
                #'CubeMapCompact', \
                'Pyramidal', \
                'RhombicDodeca']:
            storageName = '{}/{}{}_storage.dat'.format(self.outputDir,layoutId,qecId)
            layoutVideoName = '{}/{}{}.mkv'.format(self.outputDir,layoutId,qecId)
            ls = LayoutGenerators.LayoutStorage.Load(storageName)
            if self.reuseVideo:
                inputVideos.append(layoutVideoName)
                layoutsToTest.append( [(ls.layout, ls.a)] )
            else:
                inputVideos.append(self.inputVideo)
                layoutsToTest.append( [(eqL, None),(ls.layout, ls.a)] )
    
        #Test Layout
        flatFixedLayout = LayoutGenerators.FlatFixedLayout('FlatFixed{}_{}'.format(abs(cy),abs(cp)).replace('.','_'), self.outputResolution[0], self.outputResolution[1], 110, center)
        GenerateVideo.ComputeFlatFixedQoE(self.config, self.trans, layoutsToTest, flatFixedLayout, 24, self.n, inputVideos, outputDirQEC, currentQec, (cy, cp), goodPoint)

    def __RunTest__(self):
        workDone = False
        try:
            distList = [ min(dist, math.pi) for dist in np.arange(0, math.pi+self.distStep, self.distStep) ]
            for dist in distList:
                LayoutGenerators.FlatFixedLayout.SetRandomSeed(dist)
                k = self.job.nbTest
                while k != 0:
                    for qec in LayoutGenerators.QEC.TestQecGenerator(self.nbQec):
                        print('Start computation for QEC({}) for test id {} and distance {}'.format(qec.GetStrId(), self.job.nbTest-k, dist))
                        point = LayoutGenerators.FlatFixedLayout.GetRandomCenterAtDistance(qec, dist) #Get the good flat fixed center
                        (y,p) = point
                        if abs(dist - qec.ComputeDistance(y,p)) > 10**-5:
                            print('ERROR distance of the random point too far compare to expected distance: expect', dist, 'but got', qec.ComputeDistance(y,p))
                            quit()
        
                        self.__RunFlatFixedViewTest__(point, qec)
                    k -= 1
        
            #print Results:
            FormatResults.WriteQualityInTermsOfDistanceCSVFixedDistance('{}/distanceQuality.csv'.format(self.outputDir), self.outputDir, LayoutGenerators.QEC.TestQecGenerator(self.nbQec), distList, self.comment)
            FormatResults.WriteQualityCdfCSV('{}/cdfQuality.csv'.format(self.outputDir), self.outputDir, LayoutGenerators.QEC.TestQecGenerator(self.nbQec), self.comment)
        
            #FormatResults.GeneratePDF(self.outputDir, '{}/plots'.format(self.outputDir), self.comment)
        
            workDone = True

        #except Exception as inst:
        #    print (inst)
        except KeyboardInterrupt:
            raise
        except Exception as e:
            exc_type, exc_value, exc_traceback = sys.exc_info()
            traceback.print_exception(exc_type, exc_value, exc_traceback, limit=2, file=sys.stdout)
            raise
        finally:
            if not workDone:
                return (workDone, None)
            else:
                return (workDone, Results(self.outputDir, self.job))
            print('Job done')

class FixedAverageAndFixedDistances(GenericWorker):
    def __init__(self, trans, config, outDir, hostname):
        super().__init__(trans, config, outDir, hostname)
        #super().RunningTests = self.__RunningTests__

    def RunningTests(self):
        self.__GenerateAverageVideos__()
        self.__GenerateBaseVideos__()
        return self.__RunTest__()

    def __GenerateBaseVideos__(self):
        '''Generate the base video for the different layouts and differents QECs'''
        
        #for each QEC we compute the EquirectangularTiled layout associated + the other layout with a fixed bitrate
        for qec in LayoutGenerators.QEC.TestQecGenerator(self.nbQec):
            qecId = qec.GetStrId()
            print('Start computation for QEC({})'.format(qecId))
            outEquiTiledNameStorage = '{}/equirectangularTiled{}_storage.dat'.format(self.outputDir,qecId)
            outEquiTiledNameVideo = '{}/equirectangularTiled{}.mkv'.format(self.outputDir,qecId)
            outEquiTiledId = '{}/equirectangularTiled{}'.format(self.outputDir,qecId)
            GenerateVideo.GenerateVideoAndStore(self.config, self.trans,
                    [(LayoutGenerators.EquirectangularLayout('Equirectangular'), None),
                     (LayoutGenerators.EquirectangularTiledLayout('EquirectangularTiled{}'.format(qecId),qec, self.refWidth, self.refHeight), None)],
                    24, self.n,  self.inputVideo, outEquiTiledId, self.bitrateGoal)
        
            goalSize = os.stat(outEquiTiledNameVideo).st_size
            self.averageGoalSize = (self.averageGoalSize[0] + goalSize, self.averageGoalSize[1]+1)
        
            for (lName, lGenerator) in [('CubeMap{}'.format(qecId), (lambda n,ypr: LayoutGenerators.CubeMapLayout(n, ypr, self.refWidth, self.refHeight))),\
                    ('CubeMapCompact{}'.format(qecId), (lambda n,ypr: LayoutGenerators.CubeMapLayoutCompact(n, ypr, self.refWidth, self.refHeight))),\
                    ('Pyramidal{}'.format(qecId),  (lambda n,ypr: LayoutGenerators.PyramidLayout(n,ypr,2.5, self.refWidth, self.refHeight))),\
                    ('RhombicDodeca{}'.format(qecId), (lambda n,ypr: LayoutGenerators.RhombicDodecaLayout(n, ypr, self.refWidth, self.refHeight)))]:
                layout = lGenerator(lName, qec.GetEulerAngles())
                outLayoutId = '{}/{}'.format(self.outputDir,lName)
                #SearchTools.DichotomousSearch(trans, config, n, inputVideo, outLayoutId, goalSize, layout, maxIteration)
                GenerateVideo.GenerateVideoAndStore(self.config, self.trans,
                        [(LayoutGenerators.EquirectangularLayout('Equirectangular'), None),(layout, 0.5)], 24, self.n, self.inputVideo, outLayoutId, self.bitrateGoal)

    def __GenerateAverageVideos__(self):
        #First we re-encode the original Equirectangular video for fair comparaison later
        outEquiNameStorage = '{}/equirectangular_storage.dat'.format(self.outputDir)
        outEquiNameVideo = '{}/equirectangular.mkv'.format(self.outputDir)
        outEquiId = '{}/equirectangular'.format(self.outputDir)
        GenerateVideo.GenerateVideoAndStore(self.config, self.trans, [(LayoutGenerators.EquirectangularLayout('Equirectangular'), None)], 24, self.n,
                self.inputVideo, outEquiId, 0)
        
        #We get the resolution of the video
        ffmpegProcess = sub.Popen(['ffmpeg', '-i', outEquiNameVideo], stderr=sub.PIPE)
        regex = re.compile('.*\s(\d+)x(\d+)\s.*')
        for line in iter(ffmpegProcess.stderr.readline, b''):
            m = regex.match(line.decode('utf-8'))
            if m is not None:
                self.refWidth = int(m.group(1))
                self.refHeight = int(m.group(2))

        #Compute the average equirectangularTiled video
        averageNameStorage = '{}/AverageEquiTiled_storage.dat'.format(self.outputDir)
        self.averageNameVideo = '{}/AverageEquiTiled.mkv'.format(self.outputDir)
        skip = False
        if os.path.isfile(averageNameStorage) and os.path.isfile(self.averageNameVideo):
            self.lsAverage = LayoutGenerators.LayoutStorage.Load(averageNameStorage)
            if self.n == self.lsAverage.nbFrames:
                skip = True
        if not skip:
            outLayoutId = '{}/AverageEquiTiled'.format(self.outputDir)
            layoutAverage = LayoutGenerators.EquirectangularTiledLayout('AverageEquiTiled', None, self.refWidth, self.refHeight)
            GenerateVideo.GenerateVideoAndStore(self.config, self.trans,
              [(LayoutGenerators.EquirectangularLayout('Equirectangular'), None),(layoutAverage, 0.5117)], 24, self.n, self.inputVideo, outLayoutId, 0)
            self.lsAverage = LayoutGenerators.LayoutStorage.Load(averageNameStorage)
        goalSize = os.stat(self.averageNameVideo).st_size
        self.bitrateGoal = int(self.n*goalSize/24.0)
        self.job.bitrateGoal = self.bitrateGoal

    def __RunFlatFixedViewTest__(self, point, currentQec):
        (cy, cp) = point
        center = (cy, cp, 0)
        closestQec = LayoutGenerators.QEC.GetClosestQecFromTestQec(cy, cp, self.nbQec)
        (i,j) = currentQec.GetTileCoordinate()
        qecId = currentQec.GetStrId()
        (y,p,r) = currentQec.GetEulerAngles()
        goodPoint = (currentQec == closestQec)
        #First we check if the output folder for this QEC exist:
        outputDirQEC = '{}/QEC{}'.format(self.outputDir, qecId)
        if not os.path.isdir(outputDirQEC):
            os.makedirs(outputDirQEC)
        eqL = LayoutGenerators.EquirectangularLayout('Equirectangular')
        if self.reuseVideo:
            inputVideos = ['{}/equirectangular.mkv'.format(self.outputDir), '{}/equirectangularTiled{}.mkv'.format(self.outputDir,qecId), self.averageNameVideo]
            layoutsToTest = [[(eqL, None)], [(LayoutGenerators.EquirectangularTiledLayout('EquirectangularTiled{}'.format(qecId), currentQec, self.refWidth, self.refHeight), None)], \
                    [(self.lsAverage.layout, self.lsAverage.a)]]
        else:
            inputVideos = [self.inputVideo, self.inputVideo, self.inputVideo]
            layoutsToTest = [[(eqL, None)], [(eqL, None),(LayoutGenerators.EquirectangularTiledLayout('EquirectangularTiled{}'.format(qecId), currentQec, self.refWidth, self.refHeight), None)], \
                    [(eqL, None),(self.lsAverage.layout, self.lsAverage.a)]]
        for layoutId in ['CubeMap', \
                #'CubeMapCompact', \
                'Pyramidal', \
                'RhombicDodeca']:
            storageName = '{}/{}{}_storage.dat'.format(self.outputDir,layoutId,qecId)
            layoutVideoName = '{}/{}{}.mkv'.format(self.outputDir,layoutId,qecId)
            ls = LayoutGenerators.LayoutStorage.Load(storageName)
            if self.reuseVideo:
                inputVideos.append(layoutVideoName)
                layoutsToTest.append( [(ls.layout, ls.a)] )
            else:
                inputVideos.append(self.inputVideo)
                layoutsToTest.append( [(eqL, None),(ls.layout, ls.a)] )
    
        #Test Layout
        flatFixedLayout = LayoutGenerators.FlatFixedLayout('FlatFixed{}_{}'.format(abs(cy),abs(cp)).replace('.','_'), self.outputResolution[0], self.outputResolution[1], 110, center)
        GenerateVideo.ComputeFlatFixedQoE(self.config, self.trans, layoutsToTest, flatFixedLayout, 24, self.n, inputVideos, outputDirQEC, currentQec, (cy, cp), goodPoint)

    def __RunTest__(self):
        workDone = False
        try:
            distList = [ min(dist, math.pi) for dist in np.arange(0, math.pi+self.distStep, self.distStep) ]
            for dist in distList:
                LayoutGenerators.FlatFixedLayout.SetRandomSeed(dist)
                k = self.job.nbTest
                while k != 0:
                    for qec in LayoutGenerators.QEC.TestQecGenerator(self.nbQec):
                        print('Start computation for QEC({}) for test id {} and distance {}'.format(qec.GetStrId(), self.job.nbTest-k, dist))
                        point = LayoutGenerators.FlatFixedLayout.GetRandomCenterAtDistance(qec, dist) #Get the good flat fixed center
                        (y,p) = point
                        if abs(dist - qec.ComputeDistance(y,p)) > 10**-5:
                            print('ERROR distance of the random point too far compare to expected distance: expect', dist, 'but got', qec.ComputeDistance(y,p))
                            quit()
        
                        self.__RunFlatFixedViewTest__(point, qec)
                    k -= 1
        
            #print Results:
            FormatResults.WriteQualityInTermsOfDistanceCSVFixedDistance('{}/distanceQuality.csv'.format(self.outputDir), self.outputDir, LayoutGenerators.QEC.TestQecGenerator(self.nbQec), distList, self.comment)
            FormatResults.WriteQualityCdfCSV('{}/cdfQuality.csv'.format(self.outputDir), self.outputDir, LayoutGenerators.QEC.TestQecGenerator(self.nbQec), self.comment)
        
            #FormatResults.GeneratePDF(self.outputDir, '{}/plots'.format(self.outputDir), self.comment)
        
            workDone = True

        #except Exception as inst:
        #    print (inst)
        except KeyboardInterrupt:
            raise
        except Exception as e:
            exc_type, exc_value, exc_traceback = sys.exc_info()
            traceback.print_exception(exc_type, exc_value, exc_traceback, limit=2, file=sys.stdout)
            raise
        finally:
            if not workDone:
                return (workDone, None)
            else:
                return (workDone, Results(self.outputDir, self.job))
            print('Job done')
