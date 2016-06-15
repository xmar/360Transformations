import os
import DatasetReader
import numpy as np

def ComputeExpectedQoE(pathToDatasets, pathToOutput, pathToDistToQoE, pathToQec):
    #windowRange = [0.5, 1, 1.5, 2, 2.5, 3, 3.5, 4, 4.5, 5]
    windowRange = [0.5,1,2,3,5]
    qecReader = DatasetReader.QECReader(pathToQec)
    distToQoEReader = DatasetReader.DistanceToQoEReader(pathToDistToQoE)
    drDict = {}
    for (dirpath, dirnames, filenames) in os.walk(pathToDatasets):
        for filename in filenames:
            filepath = os.path.join(dirpath, filename)
            print ('Start processing of dataset: {}'.format(filepath))
            videoId = dirpath.split('/')[-1] + filename[:3]
            print('Video id = {}'.format(videoId))

            drDict[videoId] = DatasetReader.DatasetReader(filepath)
            drDict[videoId].ReadDataset()

    for window in windowRange:
        print ('Start processing of  window = {}'.format(window))
        with open('{}/qoeForWindow{}s.csv'.format(pathToOutput, window), 'w') as o:
            o.write('nbQec avgQoe minQoe maxQoe medQoe\n')
            qMin = {}
            qMax = {}
            for nbQec in range(1,33):
                if nbQec == 13:
                    continue
                dic = {}
                for videoId in drDict:
                    dr = drDict[videoId]
                    if videoId not in dic:
                        dic[videoId] = {}
                    if window not in dic[videoId]:
                        dic[videoId][window] = []
                    # r = dr.ComputeStatistic(window)
                    r = dr.ComputeAllPositionsWithTimestamp(window, qecReader, nbQec)
                    dic[videoId][window] = r
                    #globalDic[window] = r




                #compute the QoE
                qoeList = []
                layout = 'qualityCubeMapLower'

                for videoId in dic:
                    r = ComputeExpectedLiveQoE('{}/{}nbQec{}window{}.csv'.format(pathToOutput, videoId, nbQec, window), dic[videoId], distToQoEReader, layout, window)
                    qoeList += r #[sum(r)/len(r)]
                    if nbQec == 1:
                        qMin[videoId] = sum(qoeList)/len(qoeList)
                    if nbQec == 7:
                        qMax[videoId] = sum(qoeList)/len(qoeList)
                o.write('{} {} {} {} {}\n'.format(nbQec, sum(qoeList)/len(qoeList), min(qoeList), max(qoeList), np.percentile(qoeList, 50)))
        best = None
        qBest = None
        for videoId in qMin:
            q = qMax[videoId]-qMin[videoId]
            if qBest is None or qBest < q:
                qBest = q
                best = videoId
        print (best, qBest)


def ComputeExpectedLiveQoE(path, dic, distToQoEReader, layout, window):
    qoeList = []
    with open(path, 'w') as o:
        o.write('timestamp dist\n')
        for timestamp in sorted(dic[window].keys()):
            dist = dic[window][timestamp]
            qoe = distToQoEReader.GetQoE(dist, layout)
            o.write('{} {}\n'.format(timestamp, dist))
            qoeList.append(qoe)
    return qoeList
