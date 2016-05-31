#!/usr/bin/env python3

import argparse
import configparser
import os

import MultiProcess
import FormatResults

def SetDefaultValue(config, section):
    if 'nbQEC' not in config[section]:
        config[section]['nbQEC'] = '16'
    if 'distStep' not in config[section]:
        config[section]['distStep'] = '0.5'
    if 'nbT' not in config[section]:
        config[section]['nbT'] = '100'
    if 'n' not in config[section]:
        config[section]['n'] = '25'
    if 'r' not in config[section]:
        config[section]['r'] = '1920x1080'
    if 'bitrateGoal' not in config[section]:
        config[section]['bitrateGoal'] = '0'
    if 'reuse' not in config[section]:
        config[section]['reuse'] = 'True'
    if 'i' not in config[section]:
        config[section]['i'] = '5'
    if 'averageEqTileRatio' not in config[section]:
        config[section]['averageEqTileRatio'] = '0.5117'

def main():
    parser = argparse.ArgumentParser(description="Will test mulitple resolution and return the resolution that give a the file size closer to the goad file size");

    parser.add_argument('configFile', type=str, help='path to the configuration file')


    parser.add_argument('outputDir', type=str, help='path to outputDir' )
    parser.add_argument('-serverPort', type=int, help='server listen port', default=5042)
    parser.add_argument('authkey', type=str, help='Authentification key')
    args = parser.parse_args()

    #read the configuration file
    config = configparser.ConfigParser()
    config.read(args.configFile)

    #init some local variables
    jobList = []
    videoDic = {}
    listenPort = args.serverPort+1

    #start generating the job list and video dic

    for section in config.sections():
        SetDefaultValue(config, section)
        for nbQEC in [int(nbQEC) for nbQEC in config[section]['nbQEC'].split(',')]:
            for step in [float(step) for step in config[section]['distStep'].split(',')]:
                for nbT in [int(nbT) for nbT in config[section]['nbT'].split(',')]:
                    for n in [int(n) for n in config[section]['n'].split(',')]:
                        for r in config[section]['r'].split(','):
                            for bitrateGoal in [int(bitrateGoal) for bitrateGoal in config[section]['bitrateGoal'].split(',')]:
                                for reuse in [reuse == 'True' for reuse in config[section]['reuse'].split(',')]:
                                    for averageEqTileRatio in [float(aver) for aver in config[section]['averageEqTileRatio'].split(',')]:
                                        for inputVideo in config[section]['inputVideo'].split(','):
                                            video = MultiProcess.Video(inputVideo)
                                            if not video.fileName in videoDic:
                                                video.SetTcpPort(listenPort)
                                                listenPort += 1
                                                videoDic[video.fileName] = video
                                            jobArg = MultiProcess.JobArg(nbQEC, step,
                                                nbT, n, config[section]['i'], reuse, r,
                                                videoDic[video.fileName], bitrateGoal, averageEqTileRatio)
                                            job = MultiProcess.JobFixedAverageAndFixedDistances(jobArg)
                                            outDir = '{}/{}'.format(args.outputDir, job.ToDirName())
                                            if os.path.exists('{}/cdfQuality.csv'.format(outDir)) and\
                                                    os.path.exists('{}/cdfQuality.csv'.format(outDir)) and\
                                                    os.path.exists('{}/cdfQuality.csv'.format(outDir)):
                                                print('Results for {} already exist'.format(outDir))
                                                FormatResults.GeneratePDF(outDir, '{}/plots'.format(outDir), job.ToComment())
                                            else:
                                                jobList.append( job )

    MultiProcess.RunServer(jobList, args.outputDir, (args.serverPort, args.authkey))


if __name__ == '__main__':
    main()
