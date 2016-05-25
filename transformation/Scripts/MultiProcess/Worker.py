import multiprocessing as mp
import os
import numpy as np
import subprocess as sub
import shutil
import re
import math
import queue

import SearchTools
import GenerateVideo
import LayoutGenerators
import FormatResults
from .Results import Results
import MultiProcess.Video as Video

def WorkerManager(SpecializedWorker, shared_job_q, shared_result_q, exit_event):
    """Run a unique process that will run the SpecializedWorker"""
    proc = mp.Process(target=SpecializedWorker,
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
        except Exception:
            proc.terminate()
            proc.join(5)

def FixedBitrateAndFixedDistances(trans, config, outDir, hostname, shared_job_q, shared_result_q):
    def RunFlatFixedViewTest(point, currentQec, nbQec):
        (cy, cp) = point
        center = (cy, cp, 0)
        closestQec = LayoutGenerators.QEC.GetClosestQecFromTestQec(cy, cp, nbQec)
        (i,j) = currentQec.GetTileCoordinate()
        qecId = currentQec.GetStrId()
        (y,p,r) = currentQec.GetEulerAngles()
        goodPoint = (currentQec == closestQec)
        #First we check if the output folder for this QEC exist:
        outputDirQEC = '{}/QEC{}'.format(outputDir, qecId)
        if not os.path.isdir(outputDirQEC):
            os.makedirs(outputDirQEC)
        eqL = LayoutGenerators.EquirectangularLayout('Equirectangular')
        if reuseVideo:
            inputVideos = ['{}/equirectangular.mkv'.format(outputDir), '{}/equirectangularTiled{}.mkv'.format(outputDir,qecId), averageNameVideo]
            layoutsToTest = [[(eqL, None)], [(LayoutGenerators.EquirectangularTiledLayout('EquirectangularTiled{}'.format(qecId), currentQec, refWidth, refHeight), None)], \
                    [(lsAverage.layout, lsAverage.a)]]
        else:
            inputVideos = [inputVideo, inputVideo, inputVideo]
            layoutsToTest = [[(eqL, None)], [(eqL, None),(LayoutGenerators.EquirectangularTiledLayout('EquirectangularTiled{}'.format(qecId), currentQec, refWidth, refHeight), None)], \
                    [(eqL, None),(lsAverage.layout, lsAverage.a)]]
        for layoutId in ['CubeMap', \
                #'CubeMapCompact', \
                'Pyramidal', \
                'RhombicDodeca']:
            storageName = '{}/{}{}_storage.dat'.format(outputDir,layoutId,qecId)
            layoutVideoName = '{}/{}{}.mkv'.format(outputDir,layoutId,qecId)
            ls = LayoutGenerators.LayoutStorage.Load(storageName)
            if reuseVideo:
                inputVideos.append(layoutVideoName)
                layoutsToTest.append( [(ls.layout, ls.a)] )
            else:
                inputVideos.append(inputVideo)
                layoutsToTest.append( [(eqL, None),(ls.layout, ls.a)] )
    
        #Test Layout
        flatFixedLayout = LayoutGenerators.FlatFixedLayout('FlatFixed{}_{}'.format(abs(cy),abs(cp)).replace('.','_'), outputResolution[0], outputResolution[1], 110, center)
        GenerateVideo.ComputeFlatFixedQoE(config, trans, layoutsToTest, flatFixedLayout, 24, n, inputVideos, outputDirQEC, currentQec, (cy, cp), goodPoint)

    outputVideoDir = '{}/InputVideos'.format(outDir)
    while True:
        try:
            workDone = False
            job = None
            job = shared_job_q.get_nowait()
            n = job.nbFrames
            video = job.inputVideo
            if not os.path.exists(outputVideoDir):
                os.mkdir(outputVideoDir)
            video.UpdateVideoDir(outputVideoDir)
            if not os.path.exists(video.realPath):
                Video.VideoReceiver(video, hostname)
            inputVideo = video.realPath
            maxIteration = job.nbDicothomicInteration
            reuseVideo = job.reuseVideo
            outputResolution = job.res.split('x')
            outputResolution = (int(outputResolution[0]), int(outputResolution[1]))
            k = job.nbTest
            averageGoalSize = (0,0)
            nbQec = job.nbQec
            bitrateGoal = int(job.bitrateGoal)
            distStep = job.distStep
            outputDir = '{}/{}'.format(outDir, job.ToDirName())
            if not os.path.exists(outputDir):
                os.mkdir(outputDir)

            comment = job.ToComment()
            print('Start job {}'.format(comment))

            try:
                #First we re-encode the original Equirectangular video for fair comparaison later
                outEquiNameStorage = '{}/equirectangular_storage.dat'.format(outputDir)
                outEquiNameVideo = '{}/equirectangular.mkv'.format(outputDir)
                outEquiId = '{}/equirectangular'.format(outputDir)
                GenerateVideo.GenerateVideoAndStore(config, trans, [(LayoutGenerators.EquirectangularLayout('Equirectangular'), None)], 24, n,  inputVideo, outEquiId, bitrateGoal)
        
                #We get the resolution of the video
                ffmpegProcess = sub.Popen(['ffmpeg', '-i', outEquiNameVideo], stderr=sub.PIPE)
                regex = re.compile('.*\s(\d+)x(\d+)\s.*')
                for line in iter(ffmpegProcess.stderr.readline, b''):
                    m = regex.match(line.decode('utf-8'))
                    if m is not None:
                        refWidth = int(m.group(1))
                        refHeight = int(m.group(2))
        
                #for each QEC we compute the EquirectangularTiled layout associated + the other layout with a fixed bitrate
                for qec in LayoutGenerators.QEC.TestQecGenerator(nbQec):
                    qecId = qec.GetStrId()
                    print('Start computation for QEC({})'.format(qecId))
                    outEquiTiledNameStorage = '{}/equirectangularTiled{}_storage.dat'.format(outputDir,qecId)
                    outEquiTiledNameVideo = '{}/equirectangularTiled{}.mkv'.format(outputDir,qecId)
                    outEquiTiledId = '{}/equirectangularTiled{}'.format(outputDir,qecId)
                    GenerateVideo.GenerateVideoAndStore(config, trans, [(LayoutGenerators.EquirectangularLayout('Equirectangular'), None),(LayoutGenerators.EquirectangularTiledLayout('EquirectangularTiled{}'.format(qecId), qec, refWidth, refHeight), None)], 24, n,  inputVideo, outEquiTiledId, bitrateGoal)
        
                    goalSize = os.stat(outEquiTiledNameVideo).st_size
                    averageGoalSize = (averageGoalSize[0] + goalSize, averageGoalSize[1]+1)
        
                    for (lName, lGenerator) in [('CubeMap{}'.format(qecId), (lambda n,ypr: LayoutGenerators.CubeMapLayout(n, ypr, refWidth, refHeight))),\
                            ('CubeMapCompact{}'.format(qecId), (lambda n,ypr: LayoutGenerators.CubeMapLayoutCompact(n, ypr, refWidth, refHeight))),\
                            ('Pyramidal{}'.format(qecId),  (lambda n,ypr: LayoutGenerators.PyramidLayout(n,ypr,2.5, refWidth, refHeight))),\
                            ('RhombicDodeca{}'.format(qecId), (lambda n,ypr: LayoutGenerators.RhombicDodecaLayout(n, ypr, refWidth, refHeight)))]:
                        layout = lGenerator(lName, qec.GetEulerAngles())
                        outLayoutId = '{}/{}'.format(outputDir,lName)
                        #SearchTools.DichotomousSearch(trans, config, n, inputVideo, outLayoutId, goalSize, layout, maxIteration)
                        GenerateVideo.GenerateVideoAndStore(config, trans, [(LayoutGenerators.EquirectangularLayout('Equirectangular'), None),(layout, 0.5)], 24, n, inputVideo, outLayoutId, bitrateGoal)
                #Compute the average equirectangularTiled video
                averageGoalSize = averageGoalSize[0]/averageGoalSize[1]
                averageNameStorage = '{}/AverageEquiTiled_storage.dat'.format(outputDir)
                averageNameVideo = '{}/AverageEquiTiled.mkv'.format(outputDir)
                skip = False
                if os.path.isfile(averageNameStorage) and os.path.isfile(averageNameVideo):
                    lsAverage = LayoutGenerators.LayoutStorage.Load(averageNameStorage)
                    if n == lsAverage.nbFrames:
                        skip = True
                if not skip:
                    outLayoutId = '{}/AverageEquiTiled'.format(outputDir)
                    layoutAverage = LayoutGenerators.EquirectangularTiledLayout('AverageEquiTiled', None, refWidth, refHeight)
                    SearchTools.DichotomousSearch(trans, config, n, inputVideo, outLayoutId, averageGoalSize, layoutAverage, maxIteration)
                    lsAverage = LayoutGenerators.LayoutStorage.Load(averageNameStorage)
        
                ##Comupute results for perfect Good and Perfect Bad
                #for qec in LayoutGenerators.QEC.TestQecGenerator(nbQec):
                #    print('Start Good/Bad computation for QEC({})'.format(qec.GetStrId()))
                #    (y,p,r) = qec.GetEulerAngles()
                #    good = (y,p)
                #    bad = ( y + 180, -p )
                #    RunFlatFixedViewTest(good, qec, nbQec)
                #    RunFlatFixedViewTest(bad, qec, nbQec)
                #Now all the video representations have been generated: we start to compute the flat fixed view
                distList = [ min(dist, math.pi) for dist in np.arange(0, math.pi+distStep, distStep) ]
                for dist in distList:
                    LayoutGenerators.FlatFixedLayout.SetRandomSeed(dist)
                    k = job.nbTest
                    while k != 0:
                        for qec in LayoutGenerators.QEC.TestQecGenerator(nbQec):
                            print('Start computation for QEC({}) for test id {} and distance {}'.format(qec.GetStrId(), job.nbTest-k, dist))
                            point = LayoutGenerators.FlatFixedLayout.GetRandomCenterAtDistance(qec, dist) #Get the good flat fixed center
                            (y,p) = point
                            if abs(dist - qec.ComputeDistance(y,p)) > 10**-5:
                                print('ERROR distance of the random point too far compare to expected distance: expect', dist, 'but got', qec.ComputeDistance(y,p))
                                quit()
        
                            RunFlatFixedViewTest(point, qec, nbQec)
                        k -= 1
        
                #print Results:
                FormatResults.WriteQualityInTermsOfDistanceCSVFixedDistance('{}/distanceQuality.csv'.format(outputDir), outputDir, LayoutGenerators.QEC.TestQecGenerator(nbQec), distList, comment)
                FormatResults.WriteQualityCdfCSV('{}/cdfQuality.csv'.format(outputDir), outputDir, LayoutGenerators.QEC.TestQecGenerator(nbQec), comment)
        
                FormatResults.GeneratePDF(outputDir, '{}/plots'.format(outputDir), comment)

                workDone = True
                shared_result_q.put(Results(outputDir, job))
        
            #except Exception as inst:
            #    print (inst)
            except KeyboardInterrupt:
                raise
            finally:
                print('Job done')
                if job is not None:
                    shared_job_q.task_done()
                    if not workDone:
                        #if we fail to perform the job then we put it back into the job queue
                        shared_job_q.put(job)
        except KeyboardInterrupt:
            raise
        except queue.Empty:
            return

