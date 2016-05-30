import GenerateVideo
import os
import numpy as np

def WriteQualityInTermsOfDistanceCSV(outputPath, outputDir, qecList, commentString = None):
    distanceToQualityRAW = {}
    maxDist = 0
    for qec in qecList:
        qecId = qec.GetStrId()
        outputDirQEC = '{}/QEC{}'.format(outputDir, qecId)
        qualityStorage = '{}/quality_storage.dat'.format(outputDirQEC)
        if os.path.isdir(outputDirQEC) and os.path.isfile(qualityStorage):
            qualityStorage = GenerateVideo.QualityStorage.Load(qualityStorage)
            for lId in qualityStorage.goodQuality:
                name = lId[:-len(qecId)] if 'AverageEq' not in lId else lId
                if name not in distanceToQualityRAW:
                    distanceToQualityRAW[name] = {}
                for (q, qec, rot) in qualityStorage.goodQuality[lId]:
                    distance = qec.ComputeDistance(rot)
                    maxDist = max(maxDist, distance)
                    if distance in distanceToQualityRAW[name]:
                        print ('ERROR distance already exist')
                    distanceToQualityRAW[name][distance] = q
            for lId in qualityStorage.badQuality:
                name = lId[:-len(qecId)] if 'AverageEq' not in lId else lId
                if name not in distanceToQualityRAW:
                    distanceToQualityRAW[name] = {}
                for (q, qec, rot) in qualityStorage.badQuality[lId]:
                    distance = qec.ComputeDistance(rot)
                    maxDist = max(maxDist, distance)
                    if distance in distanceToQualityRAW[name]:
                        print ('ERROR distance already exist')
                    distanceToQualityRAW[name][distance] = q

    nbPoint = 13
    with open(outputPath, 'w') as o:
        if commentString is not None:
            o.write('%{}\n'.format(commentString))
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
                distanceToQualityProcessed[name][index] = (v+q[0], c+1)
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

    with open(outputPath[:-4]+'_psnr.csv', 'w') as o:
        if commentString is not None:
            o.write('%{}\n'.format(commentString))
        o.write('distance')
        for name in sorted(distanceToQualityRAW.keys()):
            if name != 'AverageEquiTiled' :
                o.write(' quality{}'.format(name))
        o.write('\n')
        distanceToQualityProcessed = {}
        for name in sorted(distanceToQualityRAW.keys()):
            if name != 'AverageEquiTiled':
                distanceToQualityProcessed[name] = {}
                for (d,q) in distanceToQualityRAW[name].items():
                    index = int(nbPoint*d/maxDist)
                    if index not in distanceToQualityProcessed[name]:
                        distanceToQualityProcessed[name][index] = (0,0)
                    (v,c) = distanceToQualityProcessed[name][index]
                    distanceToQualityProcessed[name][index] = (v+q[1]-distanceToQualityRAW['AverageEquiTiled'][d][1], c+1)
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

def WriteQualityInTermsOfDistanceCSVFixedDistance(outputPath, outputDir, qecList, distanceList, commentString = None):
    distanceToQualityRAW = {}
    for qec in qecList:
        qecId = qec.GetStrId()
        outputDirQEC = '{}/QEC{}'.format(outputDir, qecId)
        qualityStorage = '{}/quality_storage.dat'.format(outputDirQEC)
        if os.path.isdir(outputDirQEC) and os.path.isfile(qualityStorage):
            qualityStorage = GenerateVideo.QualityStorage.Load(qualityStorage)
            for lId in qualityStorage.goodQuality:
                name = lId[:-len(qecId)] if 'AverageEq' not in lId else lId
                if name not in distanceToQualityRAW:
                    distanceToQualityRAW[name] = {}
                for (q, qec, rot) in qualityStorage.goodQuality[lId]:
                    distance = qec.ComputeDistance(rot)
                    matchInListDist = None
                    for d in distanceList:
                        if abs(distance-d) < 10**-5:
                            matchInListDist = d
                            break
                    if matchInListDist is not None:
                        if matchInListDist not in distanceToQualityRAW[name]:
                            distanceToQualityRAW[name][matchInListDist] = []
                        distanceToQualityRAW[name][matchInListDist].append(q)
                    else:
                        print('No match for distance ',distance, ' for qec ',qecId, 'and  name ', name, ' rot ', rot.GetStrId())
                        print('qec ', qec.rotation.GetStrId(), ' rot ', rot.GetStrId())
            for lId in qualityStorage.badQuality:
                name = lId[:-len(qecId)] if 'AverageEq' not in lId else lId
                if name not in distanceToQualityRAW:
                    distanceToQualityRAW[name] = {}
                for (q, qec, rot) in qualityStorage.badQuality[lId]:
                    distance = qec.ComputeDistance(rot)
                    matchInListDist = None
                    for d in distanceList:
                        if abs(distance-d) < 10**-5:
                            matchInListDist = d
                            break
                    if matchInListDist is not None:
                        if matchInListDist not in distanceToQualityRAW[name]:
                            distanceToQualityRAW[name][matchInListDist] = []
                        distanceToQualityRAW[name][matchInListDist].append(q)

    with open(outputPath, 'w') as o:
        if commentString is not None:
            o.write('%{}\n'.format(commentString))
        o.write('distance')
        for name in sorted(distanceToQualityRAW.keys()):
            o.write(' quality{}'.format(name))
        o.write('\n')

        for dist in distanceList:
            o.write('{}'.format(dist))
            for name in sorted(distanceToQualityRAW.keys()):
                if dist in distanceToQualityRAW[name]:
                    msssimList = [ q[0] for q in distanceToQualityRAW[name][dist] ]
                    avgMsssim = sum(msssimList)/len(msssimList)
                else:
                    print('Name: ',name, 'dist ', dist ,' not in ', distanceToQualityRAW[name])
                    avgMsssim = -1
                o.write(' {}'.format(avgMsssim))
            o.write('\n')



    with open(outputPath[:-4]+'_psnr.csv', 'w') as o:
        if commentString is not None:
            o.write('%{}\n'.format(commentString))
        o.write('distance')
        for name in sorted(distanceToQualityRAW.keys()):
            if name != 'AverageEquiTiled' :
                o.write(' quality{}'.format(name))
        o.write('\n')

        for dist in distanceList:
            o.write('{}'.format(dist))
            if dist in distanceToQualityRAW['AverageEquiTiled']:
                psnrList = [ q[1] for q in distanceToQualityRAW['AverageEquiTiled'][dist] ]
                avgPsnrForAvgVid = sum(psnrList)/len(psnrList)
                for name in sorted(distanceToQualityRAW.keys()):
                    if name != 'AverageEquiTiled':
                        if dist in distanceToQualityRAW[name]:
                            psnrList = [ q[1] for q in distanceToQualityRAW[name][dist] ]
                            avgPsnr = sum(psnrList)/len(psnrList)
                        else:
                            avgPsnr = -1
                        o.write(' {}'.format(avgPsnr-avgPsnrForAvgVid))
            else:
                for name in sorted(distanceToQualityRAW.keys()):
                    if name != 'AverageEquiTiled' :
                        o.write(' -1')
            o.write('\n')



def WriteQualityCdfCSV(outputPath, outputDir, qecList, commentString = None):
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
                name = lId[:-len(qecId)] if 'AverageEq' not in lId else lId
                if name not in goodQuality:
                    goodQuality[name] = []
                for (q, qec, rot) in qualityStorage.goodQuality[lId]:
                    goodQuality[name].append(q[0])
            for lId in qualityStorage.badQuality:
                name = lId[:-len(qecId)] if 'AverageEq' not in lId else lId
                if name not in badQuality:
                    badQuality[name] = []
                for (q, qec, rot) in qualityStorage.badQuality[lId]:
                    badQuality[name].append(q[0])

    for name in goodQuality:
        goodQuality[name] = np.array(sorted(goodQuality[name]))
    for name in badQuality:
        badQuality[name] = np.array(sorted(badQuality[name]))

    with open(outputPath, 'w') as o:
        if commentString is not None:
            o.write('%{}\n'.format(commentString))
        o.write('cdf')
        for name in sorted(goodQuality.keys()):
            o.write(' good{0} bad{0}'.format(name))
        o.write('\n')
        for r in np.arange(0,100.5, 0.5):
            o.write('{}'.format(r))
            for name in sorted(goodQuality.keys()):
                qGood = np.percentile(goodQuality[name], r)
                qBad = np.percentile(badQuality[name], r) if name in badQuality else -1
                o.write(' {} {}'.format(qGood, qBad))
            o.write('\n')
