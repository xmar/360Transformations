import os
import sys
import numpy as np
import subprocess as sub
import shutil

import LayoutGenerators
import GenerateVideo

def DichotomousSearch(trans, config, n, inputVideo, outputName, goalSize, layout, maxIteration):
    best = 0
    bestSize = None

    count = maxIteration
    a_min = 0
    a_max = 1
    print('The goal size is ',goalSize, 'Bytes')

    try:
        if os.path.isfile(outputName+'_storage.dat') and os.path.isfile(outputName+'.mkv'):
            ls = LayoutGenerators.LayoutStorage.Load(outputName+'_storage.dat')
            if ls.nbFrames == n:
                count = 0
                best = ls.a
                bestSize = abs(os.stat(outputName+'.mkv').st_size-goalSize)
        while count != 0:
            count -= 1
            a = (a_max+a_min)/2.0
            print ('Start test for a = ',a, ', a_min = ',a_min, ', a_max = ', a_max)
    
            layouts_a = [(LayoutGenerators.EquirectangularLayout('Equirectangular'), None), (layout, a)]
            GenerateVideo.GenerateVideo(config, trans, layouts_a, 24, n,  inputVideo, '/tmp/search')
    
    
            size = os.stat('/tmp/search.mkv').st_size
            diffSize = abs(goalSize-size)
            if bestSize is None or bestSize > diffSize:
                bestSize = diffSize
                best = a
                shutil.copyfile('/tmp/search.mkv', outputName+'.mkv')
                shutil.copyfile(config, outputName+'_log.txt')
            print ('Current brest is a = ', best, ', diffsize = ', bestSize, ' bytes')
            if (size > goalSize):
                a_max = a
            elif (size < goalSize):
                a_min = a
            else:
                break
    
        print('Best = ',best, ', diffsize = ', bestSize, ' bytes')
        ls = LayoutGenerators.LayoutStorage(layout, best, n)
        ls.Dump(outputName+'_storage.dat')
    except KeyboardInterrupt:
        print('Received <ctrl-c>')
        sys.exit(1)
    finally:
        if os.path.isfile('/tmp/search.mkv'):
            os.remove('/tmp/search.mkv')
        if os.path.isfile(config):
            os.remove(config)
