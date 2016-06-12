import os
import DatasetReader
import numpy as np

def ComputeExpectedQoE(pathToDatasets, pathToOutput, pathToDistToQoE):
    dic = {}
    globalDic = {}
    windowRange = [0.5, 1, 1.5, 2, 2.5, 3, 3.5, 4, 4.5, 5]
    for window in windowRange:
        for (dirpath, dirnames, filenames) in os.walk(pathToDatasets):
            for filename in filenames:
                filepath = os.path.join(dirpath, filename)
                print ('Start processing of dataset: {}'.format(filepath))
                print ('window = {}'.format(window))
                videoId = dirpath.split('/')[-1] + filename[:-4]
                print('Video id = {}'.format(videoId))

                dr = DatasetReader.DatasetReader(filepath)
                dr.ReadDataset()
                if videoId not in dic:
                    dic[videoId] = {}
                if window not in dic[videoId]:
                    dic[videoId][window] = []
                if window not in globalDic:
                    globalDic[window] = []
                # r = dr.ComputeStatistic(window)
                r = dr.ComputeAllPositionsWithTimestamp(window)
                dic[videoId][window] = r
                #globalDic[window] = r

    distToQoEReader = DatasetReader.DistanceToQoEReader(pathToDistToQoE)
    #compute the QoE
    for window in sorted(windowRange):
        for layout in distToQoEReader.layoutToId.keys():
            for videoId in dic:
                print ("Compute CDF for video {}_expectedLiveQoE_{}s_{}".format(videoId, window, layout))
                WriteExpectedLiveQoE('{}/{}_expectedLiveQoE_{}s_{}.csv'.format(pathToOutput, videoId, window, layout), dic[videoId], distToQoEReader, layout, window)
            #print ("Compute CDF for global")
            #WriteExpectedLiveQoE('{}/global_expectedLiveQoE_{}s_{}.csv'.format(pathToOutput, window, layout), globalDic, distToQoEReader, layout, window)

def WriteExpectedLiveQoE(path, dic, distToQoEReader, layout, window):
    with open(path, 'w') as o:
        o.write('timestamp dist qoe\n')
        for timestamp in sorted(dic[window].keys()):
            dist = dic[window][timestamp]
            qoe = distToQoEReader.GetQoE(dist, layout)
            o.write('{} {} {}\n'.format(timestamp, dist, qoe))
