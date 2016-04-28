#!/usr/bin/env python3

import argparse
import os
import numpy as np
import subprocess as sub
import shutil

import SearchTools
import GenerateVideo
import LayoutGenerators
import FormatResults

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

    try:
        for qec in LayoutGenerators.QEC.TestQecGenerator():
            (i,j) = qec.GetTileCoordinate()
            print('Start computation for QEC({},{})'.format(i,j))
            qec = LayoutGenerators.QEC(i,j)
            outEquiNameStorage = '{}/equirectangularTiled{}_{}_storage.dat'.format(outputDir,i,j)
            outEquiNameVideo = '{}/equirectangularTiled{}_{}.mkv'.format(outputDir,i,j)
            outEquiId = '{}/equirectangularTiled{}_{}'.format(outputDir,i,j)
            skip = False
            if os.path.isfile(outEquiNameStorage) and os.path.isfile(outEquiNameVideo):
                ls = LayoutGenerators.LayoutStorage.Load(outEquiNameStorage)
                if n == ls.nbFrames:
                    skip = True
            if not skip:
                ls = GenerateVideo.GenerateVideo(config, trans, [(LayoutGenerators.EquirectangularLayout('Equirectangular'), None),(LayoutGenerators.EquirectangularTiledLayout('EquirectangularTiled{}_{}'.format(i,j), qec), None)], 24, n,  inputVideo, outEquiId)
                ls.Dump(outEquiNameStorage)
            
            goalSize = os.stat(outEquiNameVideo).st_size
            averageGoalSize = (averageGoalSize[0] + goalSize, averageGoalSize[1]+1)
  
            for (lName, lGenerator) in [('CubMap{}_{}'.format(i,j), LayoutGenerators.CubeMapLayout),\
                    ('CubMapCompact{}_{}'.format(i,j), LayoutGenerators.CubeMapLayoutCompact),\
                    ('Pyramidal{}_{}'.format(i,j),  (lambda n,ypr: LayoutGenerators.PyramidLayout(n,ypr,3))),\
                    ('RhombicDodeca{}_{}'.format(i,j), LayoutGenerators.RhombicDodecaLayout)]:
                layout = lGenerator(lName, qec.GetEulerAngles())
                outLayoutId = '{}/{}'.format(outputDir,lName)
                SearchTools.DichotomousSearch(trans, config, n, inputVideo, outLayoutId, goalSize, layout, maxIteration)
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
            layoutAverage = LayoutGenerators.EquirectangularTiledLayout('AverageEquiTiled', None) 
            SearchTools.DichotomousSearch(trans, config, n, inputVideo, outLayoutId, averageGoalSize, layoutAverage, maxIteration)
            lsAverage = LayoutGenerators.LayoutStorage.Load(averageNameStorage)

        #Now all the video representations have been generated: we start to compute the flat fixed view
        while k != 0:
            (cy, cp) = LayoutGenerators.FlatFixedLayout.GetRandomCenter() #Get the good flat fixed center
            (cyBad, cpBad) = LayoutGenerators.FlatFixedLayout.GetRandomCenter() #Get a badly located flat fixed center
            goodCenter = (cy, cp, 0)
            badCenter = (cyBad, cpBad, 0)
            closestQec = LayoutGenerators.QEC.GetClosestQecFromTestQec(cy, cp)
            (i,j) = closestQec.GetTileCoordinate()
            (y,p,r) = closestQec.GetEulerAngles()
            #First we check if the output folder for this QEC exist:
            outputDirQEC = '{}/QEC{}_{}'.format(outputDir, i, j)
            if not os.path.isdir(outputDirQEC):
                os.makedirs(outputDirQEC)
            eqL = LayoutGenerators.EquirectangularLayout('Equirectangular')
            if reuseVideo:
                inputVideos = [inputVideo, '{}/equirectangularTiled{}_{}.mkv'.format(outputDir,i,j), averageNameVideo]
                layoutsToTest = [[(eqL, None)], [(LayoutGenerators.EquirectangularTiledLayout('EquirectangularTiled{}_{}'.format(i,j), closestQec), None)], \
                        [(lsAverage.layout, lsAverage.a)]]
            else:
                inputVideos = [inputVideo, inputVideo, inputVideo]
                layoutsToTest = [[(eqL, None)], [(eqL, None),(LayoutGenerators.EquirectangularTiledLayout('EquirectangularTiled{}_{}'.format(i,j), closestQec), None)], \
                        [(eqL, None),(lsAverage.layout, lsAverage.a)]]
            for layoutId in ['CubMap', \
                    #'CubMapCompact', \
                    'Pyramidal', \
                    'RhombicDodeca']:
                storageName = '{}/{}{}_{}_storage.dat'.format(outputDir,layoutId,i,j)
                layoutVideoName = '{}/{}{}_{}.mkv'.format(outputDir,layoutId,i,j)
                ls = LayoutGenerators.LayoutStorage.Load(storageName)
                if reuseVideo:
                    inputVideos.append(layoutVideoName)
                    layoutsToTest.append( [(ls.layout, ls.a)] )
                else:
                    inputVideos.append(inputVideo)
                    layoutsToTest.append( [(eqL, None),(ls.layout, ls.a)] )

            #Test Good Layout
            flatFixedLayout = LayoutGenerators.FlatFixedLayout('FlatFixed{}_{}'.format(abs(cy),abs(cp)).replace('.',''), outputResolution[0], outputResolution[1], 110, goodCenter)
            GenerateVideo.ComputeFlatFixedQoE(config, trans, layoutsToTest, flatFixedLayout, 24, n, inputVideos, outputDirQEC, closestQec, (cy, cp), True)

            #Test Bad layout
            flatFixedLayout = LayoutGenerators.FlatFixedLayout('FlatFixed{}_{}'.format(abs(cyBad),abs(cpBad)).replace('.',''), outputResolution[0], outputResolution[1], 110, badCenter)
            GenerateVideo.ComputeFlatFixedQoE(config, trans, layoutsToTest, flatFixedLayout, 24, n, inputVideos, outputDirQEC, closestQec, (cyBad, cpBad), False)

            k -= 1
        
        #print Results:
#        for qec in LayoutGenerators.QEC.TestQecGenerator():
#            (i,j) = qec.GetTileCoordinate()
#            outputDirQEC = '{}/QEC{}_{}'.format(outputDir, i, j)
#            qualityStorage = '{}/quality_storage.dat'.format(outputDirQEC)
#            if os.path.isdir(outputDirQEC) and os.path.isfile(qualityStorage):
#                qs = GenerateVideo.QualityStorage.Load(qualityStorage)
#                print('Results QEC ({},{}):'.format(i,j))
#                print('Nb test = {}'.format(len(qs.names)/2))
#                print('Good:')
#                for k in qs.goodQuality:
#                    print('Average MS-SSIM for {} = {}'.format(k, sum([p[0] for p in qs.goodQuality[k]])/len(qs.goodQuality[k])))
#                print ('Bad:')
#                for k in qs.badQuality:
#                    print('Average MS-SSIM for {} = {}'.format(k, sum([p[0] for p in qs.badQuality[k]])/len(qs.badQuality[k])))
        FormatResults.WriteQualityInTermsOfDistanceCSV('{}/distanceQuality.csv'.format(outputDir), outputDir, LayoutGenerators.QEC.TestQecGenerator())
        FormatResults.WriteQualityCdfCSV('{}/cdfQuality.csv'.format(outputDir), outputDir, LayoutGenerators.QEC.TestQecGenerator())

    except Exception as inst:
        print (inst)
    finally:
        print('Program done')
