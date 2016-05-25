#!/usr/bin/env python3

import argparse
import os

import MultiProcess
import FormatResults

def main():
    parser = argparse.ArgumentParser(description="Will test mulitple resolution and return the resolution that give a the file size closer to the goad file size");
    parser.add_argument('inputVideo', type=str, help='path to input video' )
    parser.add_argument('outputDir', type=str, help='path to outputDir' )
#    parser.add_argument('goalFile', type=str, help='path to the goal video file' )
    parser.add_argument('--reuseVideosForQuality', type=str, help='To know if reuse the video previously generated as input to compute the Quality [True]', default='True')
    parser.add_argument('-n', type=str, help='number of frame to process [50]', default='50')
    parser.add_argument('-i', type=int, help='number max of iteration for the dichotomous search [10]', default='10')
    parser.add_argument('-r', type=str, help='Output flat fixed view resolution [1920x1080]', default='1920x1080')
    parser.add_argument('-nbT', type=str, help='Number of "random" test to do [100]', default='100')
    parser.add_argument('-nbQEC', type=str, help='Number of QEC [16]', default='16')
    parser.add_argument('-step', type=str, help='Step for the distance', default ='0.5')
    parser.add_argument('-bitrateGoal', type=str, help='Bitrate goal', default ='10000')
    parser.add_argument('-serverPort', type=int, help='server listen port', default=5042)
    parser.add_argument('authkey', type=str, help='Authentification key')
    args = parser.parse_args()

    jobList = []
    videoDic = {}
    listenPort = args.serverPort+1
    for nbQEC in [int(nbQEC) for nbQEC in args.nbQEC.split(',')]:
        for step in [float(step) for step in args.step.split(',')]:
            for nbT in [int(nbT) for nbT in args.nbT.split(',')]:
                for n in [int(n) for n in args.n.split(',')]:
                    for r in args.r.split(','):
                        for bitrateGoal in [int(bitrateGoal) for bitrateGoal in args.bitrateGoal.split(',')]:
                            for reuse in [reuse == 'True' for reuse in args.reuseVideosForQuality.split(',')]:
                                for inputVideo in args.inputVideo.split(','):
                                    video = MultiProcess.Video(inputVideo)
                                    if not video.fileName in videoDic:
                                        video.SetTcpPort(listenPort)
                                        listenPort += 1
                                        videoDic[video.fileName] = video
                                    job = MultiProcess.Job(nbQEC, step, nbT, n, args.i, reuse, r, videoDic[video.fileName], bitrateGoal)
                                    outDir = '{}/{}'.format(args.outputDir, job.ToDirName())
                                    if os.path.exists('{}/cdfQuality.csv'.format(outDir)) and\
                                            os.path.exists('{}/cdfQuality.csv'.format(outDir)) and\
                                            os.path.exists('{}/cdfQuality.csv'.format(outDir)):
                                        FormatResults.GeneratePDF(outDir, '{}/plots'.format(outDir), job.ToComment())
                                    else:
                                        jobList.append( job )

    MultiProcess.RunServer(jobList, args.outputDir, (args.serverPort, args.authkey))


if __name__ == '__main__':
    main()
