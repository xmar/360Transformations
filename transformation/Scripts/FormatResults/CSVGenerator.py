import GenerateVideo
import os
import numpy as np

def WriteQualityInTermsOfDistanceCSV(outputPath, outputDir, qecList):
    distanceToQualityRAW = {}
    maxDist = 0
    for qec in qecList:
        qecId = qec.GetStrId()
        outputDirQEC = '{}/QEC{}'.format(outputDir, qecId)
        qualityStorage = '{}/quality_storage.dat'.format(outputDirQEC)
        if os.path.isdir(outputDirQEC) and os.path.isfile(qualityStorage):
            qualityStorage = GenerateVideo.QualityStorage.Load(qualityStorage)
            for lId in qualityStorage.goodQuality:
                name = lId[:-3]
                if name not in distanceToQualityRAW:
                    distanceToQualityRAW[name] = {}
                for (q, qec, (y,p)) in qualityStorage.goodQuality[lId]:
                    distance = qec.ComputeDistance(y,p)
                    maxDist = max(maxDist, distance)
                    distanceToQualityRAW[name][distance] = q
            for lId in qualityStorage.badQuality:
                name = lId[:-3]
                if name not in distanceToQualityRAW:
                    distanceToQualityRAW[name] = {}
                for (q, qec, (y,p)) in qualityStorage.badQuality[lId]:
                    distance = qec.ComputeDistance(y,p)
                    maxDist = max(maxDist, distance)
                    distanceToQualityRAW[name][distance] = q

    nbPoint = 20
    with open(outputPath, 'w') as o:
        o.write('distance')
        for name in sorted(distanceToQualityRAW.keys()):
            o.write(' quality{}'.format(name))
        o.write('\n')
        distanceToQualityProcessed = {}
        for name in distanceToQualityRAW:
            distanceToQualityProcessed[name] = {}
            for (d,q) in distanceToQualityRAW[name].items():
                index = int(nbPoint*d/maxDist)
                if index not in distanceToQualityProcessed[name]:
                    distanceToQualityProcessed[name][index] = (0,0)
                (v,c) = distanceToQualityProcessed[name][index]
                distanceToQualityProcessed[name][index] = (v+q, c+1)
        for i in range(0, nbPoint):
            hasValue = False
            for name in distanceToQualityProcessed:
                hasValue = hasValue or (i in distanceToQualityProcessed[name])
                if hasValue:
                    break
            if not hasValue:
                continue
            o.write('{}'.format((i+0.5)*maxDist/nbPoint))
            for name in sorted(distanceToQualityProcessed.keys()):
                if i in distanceToQualityProcessed[name]:
                    (v,c) = distanceToQualityProcessed[name][i]
                else:
                    (v,c) = (-1,1)
                o.write(' {}'.format(v/c))
            o.write('\n')


def WriteQualityCdfCSV(outputPath, outputDir, qecList):
    goodQuality = {}
    badQuality = {}
    maxDist = 0
    for qec in qecList:
        qecId = qec.GetStrId()
        outputDirQEC = '{}/QEC{}'.format(outputDir, qecId)
        qualityStorage = '{}/quality_storage.dat'.format(outputDirQEC)
        if os.path.isdir(outputDirQEC) and os.path.isfile(qualityStorage):
            qualityStorage = GenerateVideo.QualityStorage.Load(qualityStorage)
            for lId in qualityStorage.goodQuality:
                name = lId[:-3]
                if name not in goodQuality:
                    goodQuality[name] = []
                for (q, qec, (y,p)) in qualityStorage.goodQuality[lId]:
                    goodQuality[name].append(q)
            for lId in qualityStorage.badQuality:
                name = lId[:-3]
                if name not in badQuality:
                    badQuality[name] = []
                for (q, qec, (y,p)) in qualityStorage.badQuality[lId]:
                    badQuality[name].append(q)

    for name in goodQuality:
        goodQuality[name] = np.array(sorted(goodQuality[name]))
    for name in badQuality:
        badQuality[name] = np.array(sorted(badQuality[name]))

    with open(outputPath, 'w') as o:
        o.write('cdf')
        for name in sorted(goodQuality.keys()):
            o.write(' good{0} bad{0}'.format(name))
        o.write('\n')
        for r in np.arange(0,100.5, 0.5):
            o.write('{}'.format(r))
            for name in sorted(goodQuality.keys()):
                qGood = np.percentile(goodQuality[name], r)
                qBad = np.percentile(badQuality[name], r)
                o.write(' {} {}'.format(qGood, qBad))
            o.write('\n')
