import os
import numpy as np
import math

class QEC(object):
    def __init__(self, qecCoordinates):
        self.center = np.reshape(np.matrix([[qecCoordinates[0]], [qecCoordinates[1]], [qecCoordinates[2]]]), 3)

    def OrthodromicDistance(self, fovCenter):
        #print(np.inner(self.c, fovCenter.c))
        return math.acos(min(1, max(-1,np.inner(self.center, fovCenter.c))))

class QECReader(object):
    def __init__(self, pathToQECDirectory):
        self.qecs = dict() #dict qec number -> list of qec with
        for (dirpath, dirnames, filenames) in os.walk(pathToQECDirectory):
            for f in filenames:
                if f[0] == '.':
                    continue
                path = os.path.join(dirpath, f)
                nbQec = int(f[:-4])
                self.qecs[nbQec] = []
                with open(path, 'r') as i:
                    for line in i:
                        qecCoordinates = [float(coord) for coord in line.split('\t')[1:]]
                        self.qecs[nbQec].append(QEC(qecCoordinates))

    def GetClosestQEC(self, nbQec, fovCenter):
        bestQec = None
        bestDistance = None
        for qec in self.qecs[nbQec]:
            distance = qec.OrthodromicDistance(fovCenter)
            if bestDistance is None or bestDistance > distance:
                bestDistance = distance
                bestQec = qec
        return bestQec
