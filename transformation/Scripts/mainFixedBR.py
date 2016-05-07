#!/usr/bin/env python3

import argparse
import os
import numpy as np
import subprocess as sub
import shutil
import re

import SearchTools
import GenerateVideo
import LayoutGenerators
import FormatResults

def RunFlatFixedViewTest(good, bad):
    (cy, cp) = good
    (cyBad, cpBad) = bad
    goodCenter = (cy, cp, 0)
    badCenter = (cyBad, cpBad, 0)
    closestQec = LayoutGenerators.QEC.GetClosestQecFromTestQec(cy, cp)
    (i,j) = closestQec.GetTileCoordinate()
    qecId = closestQec.GetStrId()
    (y,p,r) = closestQec.GetEulerAngles()
    #First we check if the output folder for this QEC exist:
    outputDirQEC = '{}/QEC{}'.format(outputDir, qecId)
    if not os.path.isdir(outputDirQEC):
        os.makedirs(outputDirQEC)
    eqL = LayoutGenerators.EquirectangularLayout('Equirectangular')
    if reuseVideo:
        inputVideos = ['{}/equirectangular.mkv'.format(outputDir), '{}/equirectangularTiled{}.mkv'.format(outputDir,qecId), averageNameVideo]
        layoutsToTest = [[(eqL, None)], [(LayoutGenerators.EquirectangularTiledLayout('EquirectangularTiled{}'.format(qecId), closestQec, refWidth, refHeight), None)], \
                [(lsAverage.layout, lsAverage.a)]]
    else:
        inputVideos = [inputVideo, inputVideo, inputVideo]
        layoutsToTest = [[(eqL, None)], [(eqL, None),(LayoutGenerators.EquirectangularTiledLayout('EquirectangularTiled{}'.format(qecId), closestQec), None, refWidth, refHeight)], \
                [(eqL, None),(lsAverage.layout, lsAverage.a)]]
    for layoutId in ['CubMap', \
            #'CubMapCompact', \
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
    
    #Test Good Layout
    flatFixedLayout = LayoutGenerators.FlatFixedLayout('FlatFixed{}_{}'.format(abs(cy),abs(cp)).replace('.','_'), outputResolution[0], outputResolution[1], 110, goodCenter)
    GenerateVideo.ComputeFlatFixedQoE(config, trans, layoutsToTest, flatFixedLayout, 24, n, inputVideos, outputDirQEC, closestQec, (cy, cp), True)
    
    #Test Bad layout
    flatFixedLayout = LayoutGenerators.FlatFixedLayout('FlatFixed{}_{}'.format(abs(cyBad),abs(cpBad)).replace('.','_'), outputResolution[0], outputResolution[1], 110, badCenter)
    GenerateVideo.ComputeFlatFixedQoE(config, trans, layoutsToTest, flatFixedLayout, 24, n, inputVideos, outputDirQEC, closestQec, (cyBad, cpBad), False)


if __name__ ==  '__main__':
    parser = argparse.ArgumentParser(description="Will test mulitple resolution and return the resolution that give a the file size closer to the goad file size");
    parser.add_argument('inputVideo', type=str, help='path to input video' )
#    parser.add_argument('goalFile', type=str, help='path to the goal video file' )
    parser.add_argument('outputDir', type=str, help='path to the output dir')
    parser.add_argument('--trans',  type=str, help='path to the trans software', default='../build/trans')
    parser.add_argument('--config', type=str, help='path to the generated config file', default='./ConfigTest.ini')
    parser.add_argument('--doNotReuseVideosForQuality', help='if set, will not reuse the video previously generated as input to compute the Quality', dest='reuseVideosForQuality', action='store_false')
    parser.add_argument('-n', type=int, help='number of frame to process [50]', default=50)
    parser.add_argument('-i', type=int, help='number max of iteration for the dichotomous search [10]', default=10)
    parser.add_argument('-r', type=str, help='Output flat fixed view resolution [1920x1080]', default='1920x1080')
    parser.add_argument('-nbT', type=int, help='Number of "random" test to do [100]', default=100)
    args = parser.parse_args()

    trans = args.trans
    config = args.config
    n = args.n
    inputVideo = args.inputVideo
    outputDir = args.outputDir
    maxIteration = args.i
    reuseVideo = args.reuseVideosForQuality
    outputResolution = args.r.split('x')
    outputResolution = (int(outputResolution[0]), int(outputResolution[1]))
    k = args.nbT
    averageGoalSize = (0,0)
    bitrateGoal = 10000

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
        for qec in LayoutGenerators.QEC.TestQecGenerator():
            qecId = qec.GetStrId()
            print('Start computation for QEC({})'.format(qecId))
            outEquiTiledNameStorage = '{}/equirectangularTiled{}_storage.dat'.format(outputDir,qecId)
            outEquiTiledNameVideo = '{}/equirectangularTiled{}.mkv'.format(outputDir,qecId)
            outEquiTiledId = '{}/equirectangularTiled{}'.format(outputDir,qecId)
            GenerateVideo.GenerateVideoAndStore(config, trans, [(LayoutGenerators.EquirectangularLayout('Equirectangular'), None),(LayoutGenerators.EquirectangularTiledLayout('EquirectangularTiled{}'.format(qecId), qec, refWidth, refHeight), None)], 24, n,  inputVideo, outEquiTiledId, bitrateGoal)
            
            goalSize = os.stat(outEquiTiledNameVideo).st_size
            averageGoalSize = (averageGoalSize[0] + goalSize, averageGoalSize[1]+1)
  
            for (lName, lGenerator) in [('CubMap{}'.format(qecId), (lambda n,ypr: LayoutGenerators.CubeMapLayout(n, ypr, refWidth, refHeight))),\
                    ('CubMapCompact{}'.format(qecId), (lambda n,ypr: LayoutGenerators.CubeMapLayoutCompact(n, ypr, refWidth, refHeight))),\
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

        #Comupute results for perfect Good and Perfect Bad
        for qec in LayoutGenerators.QEC.TestQecGenerator():
            (y,p,r) = qec.GetEulerAngles()
            good = (y,p)
            bad = ( y + 180, -p )
            RunFlatFixedViewTest(good, bad)
        #Now all the video representations have been generated: we start to compute the flat fixed view
        while k != 0:
            good = LayoutGenerators.FlatFixedLayout.GetRandomCenter() #Get the good flat fixed center
            bad = LayoutGenerators.FlatFixedLayout.GetRandomCenter() #Get a badly located flat fixed center

            RunFlatFixedViewTest(good, bad)
            k -= 1
        
        #print Results:
        FormatResults.WriteQualityInTermsOfDistanceCSV('{}/distanceQuality.csv'.format(outputDir), outputDir, LayoutGenerators.QEC.TestQecGenerator())
        FormatResults.WriteQualityCdfCSV('{}/cdfQuality.csv'.format(outputDir), outputDir, LayoutGenerators.QEC.TestQecGenerator())

    #except Exception as inst:
    #    print (inst)
    finally:
        print('Program done')
