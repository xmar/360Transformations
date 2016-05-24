#!/usr/bin/env python3

import argparse

import MultiProcess

def main():
    parser = argparse.ArgumentParser(description="Will test mulitple resolution and return the resolution that give a the file size closer to the goad file size");
    parser.add_argument('inputVideo', type=str, help='path to input video' )
    parser.add_argument('outputDir', type=str, help='path to outputDir' )
#    parser.add_argument('goalFile', type=str, help='path to the goal video file' )
    parser.add_argument('--doNotReuseVideosForQuality', help='if set, will not reuse the video previously generated as input to compute the Quality', dest='reuseVideosForQuality', action='store_false')
    parser.add_argument('-n', type=int, help='number of frame to process [50]', default=50)
    parser.add_argument('-i', type=int, help='number max of iteration for the dichotomous search [10]', default=10)
    parser.add_argument('-r', type=str, help='Output flat fixed view resolution [1920x1080]', default='1920x1080')
    parser.add_argument('-nbT', type=int, help='Number of "random" test to do [100]', default=100)
    parser.add_argument('-nbQEC', type=int, help='Number of QEC [16]', default=16)
    parser.add_argument('-step', type=float, help='Step for the distance', default =0.5)
    parser.add_argument('-bitrateGoal', type=float, help='Bitrate goal', default =10000)
    parser.add_argument('-serverPort', type=int, help='server listen port', default=5042)
    parser.add_argument('authkey', type=str, help='Authentification key')
    args = parser.parse_args()

    job = MultiProcess.Job(args.nbQEC, args.step, args.nbT, args.n, args.i, args.reuseVideosForQuality, args.r, args.inputVideo, args.bitrateGoal)

    MultiProcess.RunServer([job], args.outputDir, (args.serverPort, args.authkey))


if __name__ == '__main__':
    main()
