import os
import DatasetReader
import numpy as np

def ComputeCdf(pathToDatasets, pathToOutput):
    dic = {}
    globalDic = {}
    windowRange = [0.1, 0.25, 0.5, 1, 1.5, 2, 2.5, 3, 3.5, 4, 4.5, 5]
    for window in windowRange:
        for (dirpath, dirnames, filenames) in os.walk(pathToDatasets):
            for filename in filenames:
                filepath = os.path.join(dirpath, filename)
                print ('Start processing of dataset: {}'.format(filepath))
                videoId = dirpath.split('/')[-1]
                print('Video id = {}'.format(videoId))

                dr = DatasetReader.DatasetReader(filepath)
                dr.ReadDataset()
                if videoId not in dic:
                    dic[videoId] = {}
                if window not in dic[videoId]:
                    dic[videoId][window] = []
                if window not in globalDic:
                    globalDic[window] = []
                r = dr.ComputeStatistic(window)
                dic[videoId][window] += r
                globalDic[window] += r

    #compute the cdf:
    for videoId in dic:
        WritePercentile('{}/{}.csv'.format(pathToOutput, videoId), dic[videoId])

    WritePercentile('{}/global.csv'.format(pathToOutput), globalDic)

def WritePercentile(path, dic):
    with open(path, 'w') as o:
        o.write('cdf')
        for window in sorted(dic.keys()):
            o.write(' {}s'.format(window))
        o.write('\n')
        for n in np.arange(0,100.5,0.5):
            o.write('{}'.format(n))
            for window in sorted(dic.keys()):
                r = np.percentile(dic[window], n)
                o.write(' {}'.format(r))
            o.write('\n')

