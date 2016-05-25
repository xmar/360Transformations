import subprocess as sub
import shutil
import os
import sys

import LayoutGenerators


def GenerateConfigString(layouts_a, fps, bitrate, nbFrames, inputVideo, output):
    c = '[Global]\n'
    c += 'fps= {}\n'.format(fps)
    c += 'layoutFlow= [[ "{}"'.format(inputVideo)
    for (l,a) in layouts_a:
        c += ', "{}"'.format( l.GetName() )
        first = False
    c += ']]\n'
    c += 'displayFinalPict=false\n'
    c += 'videoOutputName= {}\n'.format(output)
    c += 'videoOutputBitRate={}\n'.format(bitrate)
    c += 'qualityOutputName=\n'
    c += 'nbFrames={}\n'.format(nbFrames)

    for (l,a) in layouts_a:
        c+= l.GenerateLayout(a)
    return c

def GenerateVideo(config, trans, layouts_a, fps, nbFrames,  inputVideo, outputId, bitrate=0):
    tmpVideo = '/tmp/tmp.mkv'
    lastName = None
    lastLayout = None
    lastA = None
    for (l,a) in layouts_a:
        lastName = l.GetName()
        lastLayout = l
        lastA = a
    proc = None
    try:
        with open(config, 'w') as cf:
            cf.write(GenerateConfigString(layouts_a, fps, bitrate, nbFrames, inputVideo, tmpVideo))

        proc = sub.Popen([trans, '-c', config])
        if proc.wait() != 0:
            print('Error while processing')
            return None

        shutil.move('/tmp/tmp1{}.mkv'.format(lastName), '{}.mkv'.format(outputId))
        shutil.copy(config, '{}_log.txt'.format(outputId))

    except KeyboardInterrupt:
        print('Received <ctrl-c>')
        proc.terminate()
        sys.exit(1)
        raise
    except Exception as inst:
        print (inst)
    finally:
        if os.path.isfile('/tmp/tmp1{}.mkv'.format(lastName)):
            os.remove('/tmp/tmp1{}.mkv'.format(lastName))
        return LayoutGenerators.LayoutStorage(lastLayout, lastA, nbFrames)

def GenerateVideoAndStore(config, trans, layouts_a, fps, nbFrames,  inputVideo, outputId, bitrate=0):
    outStorage = '{}_storage.dat'.format(outputId)
    outNameVideo = '{}.mkv'.format(outputId)
    skip = False
    if os.path.isfile(outStorage) and os.path.isfile(outNameVideo):
        ls = LayoutGenerators.LayoutStorage.Load(outStorage)
        if nbFrames == ls.nbFrames:
            skip = True
    if not skip:
        ls = GenerateVideo(config, trans, layouts_a, fps, nbFrames,  inputVideo, outputId, bitrate)
        if ls != None:
            ls.Dump(outStorage)
        else:
            raise Exception('Error while processing')


