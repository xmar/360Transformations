#!/usr/bin/env python3

import argparse
import os
import numpy as np
import subprocess as sub
import shutil

import SearchTools
import GenerateVideo
import LayoutGenerators

if __name__ ==  '__main__':
    parser = argparse.ArgumentParser(description="Will test mulitple resolution and return the resolution that give a the file size closer to the goad file size");
    parser.add_argument('inputVideo', type=str, help='path to input video' )
#    parser.add_argument('goalFile', type=str, help='path to the goal video file' )
    parser.add_argument('outputDir', type=str, help='path to the output dir')
    parser.add_argument('--trans',  type=str, help='path to the trans software', default='../build/trans')
    parser.add_argument('--config', type=str, help='path to the generated config file', default='./ConfigTest.ini')
    parser.add_argument('--reuseVideosForQuality', help='if set, will reuse the video previously generated as input to compute the Quality', dest='reuseVideosForQuality', action='store_true')
    parser.add_argument('-n', type=int, help='number of frame to process', default=50)
    parser.add_argument('-i', type=int, help='number max of iteration for the dichotomous search', default=10)
    args = parser.parse_args()

    trans = args.trans
    config = args.config
    n = args.n
    inputVideo = args.inputVideo
    outputDir = args.outputDir
    maxIteration = args.i
    reuseVideo = args.reuseVideosForQuality

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
  
            for (lName, lGenerator) in [('CubMap{}_{}'.format(i,j), LayoutGenerators.CubeMapLayout),\
                    ('CubMapCompact{}_{}'.format(i,j), LayoutGenerators.CubeMapLayoutCompact),\
                    ('Pyramidal{}_{}'.format(i,j),  (lambda n,ypr: LayoutGenerators.PyramidLayout(n,ypr,3))),\
                    ('RhombicDodeca{}_{}'.format(i,j), LayoutGenerators.RhombicDodecaLayout)]:
                layout = lGenerator(lName, qec.GetEulerAngles())
                outLayoutId = '{}/{}'.format(outputDir,lName)
                SearchTools.DichotomousSearch(trans, config, n, inputVideo, outLayoutId, outEquiNameVideo, layout, maxIteration)
        #Now all the video representations have been generated: we start to compute the flat fixed view
        k = 2
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
                inputVideos = [inputVideo, '{}/equirectangularTiled{}_{}.mkv'.format(outputDir,i,j)]
                layoutsToTest = [[(eqL, None)], [(LayoutGenerators.EquirectangularTiledLayout('EquirectangularTiled{}_{}'.format(i,j), closestQec), None)]]
            else:
                inputVideos = [inputVideo, inputVideo]
                layoutsToTest = [[(eqL, None)], [(eqL, None),(LayoutGenerators.EquirectangularTiledLayout('EquirectangularTiled{}_{}'.format(i,j), closestQec), None)]]
            for layoutId in ['CubMap', 'CubMapCompact', 'Pyramidal', 'RhombicDodeca']:
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
            flatFixedLayout = LayoutGenerators.FlatFixedLayout('FlatFixed{}_{}'.format(abs(cy),abs(cp)).replace('.',''), 1920, 1080, 110, goodCenter)
            GenerateVideo.ComputeFlatFixedQoE(config, trans, layoutsToTest, flatFixedLayout, 24, n, inputVideos, outputDirQEC, True)

            #Test Bad layout
            flatFixedLayout = LayoutGenerators.FlatFixedLayout('FlatFixed{}_{}'.format(abs(cyBad),abs(cpBad)).replace('.',''), 1920, 1080, 110, badCenter)
            GenerateVideo.ComputeFlatFixedQoE(config, trans, layoutsToTest, flatFixedLayout, 24, n, inputVideos, outputDirQEC, False)

            k -= 1
        
        #print Results:
        for qec in LayoutGenerators.QEC.TestQecGenerator():
            (i,j) = qec.GetTileCoordinate()
            outputDirQEC = '{}/QEC{}_{}'.format(outputDir, i, j)
            qualityStorage = '{}/quality_storage.dat'.format(outputDirQEC)
            if os.path.isdir(outputDirQEC) and os.path.isfile(qualityStorage):
                qs = GenerateVideo.QualityStorage.Load(qualityStorage)
                print('Results QEC ({},{}):'.format(i,j))
                print('Nb test = {}'.format(len(qs.names)))
                print('Good:')
                for k in qs.goodQuality:
                    print('Average MS-SSIM for {} = {}'.format(k, sum(qs.goodQuality[k])/len(qs.goodQuality[k])))
                print ('Bad:')
                for k in qs.badQuality:
                    print('Average MS-SSIM for {} = {}'.format(k, sum(qs.badQuality[k])/len(qs.badQuality[k])))

    finally:
        print('Program done')
