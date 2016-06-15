import os
import numpy as np
import math
import random

class QEC(object):
    def __init__(self, qecCoordinates):
        self.center = np.matrix([[qecCoordinates[0]], [qecCoordinates[1]], [qecCoordinates[2]]])

    def OrthodromicDistance(self, fovCenter, rotation):
        #print(np.inner(self.c, fovCenter.c))
        return math.acos(min(1, max(-1,np.inner(np.reshape(rotation*self.center, 3), fovCenter.c))))

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

    def GetClosestQEC(self, nbQec, fovCenter, r = None):
        bestQec = None
        bestDistance = None
        if r is None:
            rotation = np.matrix('1,0,0;0,1,0;0,0,1')
        else:
            random.seed(r)
            y = math.radians(random.uniform(-180,180))
            p  = math.radians(random.uniform(-90,90))
            rotation = ToRotMat(y, p, 0)
        for qec in self.qecs[nbQec]:
            distance = qec.OrthodromicDistance(fovCenter, rotation)
            if bestDistance is None or bestDistance > distance:
                bestDistance = distance
                bestQec = qec
        return (bestQec, rotation)

def ToRotMat(yaw, pitch, roll):
    '''yaw, pitch and roll in radian'''
    cosP = math.cos(pitch)
    sinP = math.sin(pitch)
    cosY = math.cos(yaw)
    sinY = math.sin(yaw)
    cosR = math.cos(roll)
    sinR = math.sin(roll)
    mat = np.matrix('{},{},{};{},{},{};{},{},{}'.format(
            cosP * cosY,     cosY*sinP*sinR -sinY*cosR,  cosY*sinP*cosR + sinY * sinR,
            sinY*cosP,       cosY * cosR,                sinY*sinP*cosR - sinR * cosY,
            -sinP,           cosP * sinR,                cosP * cosR))
    mat[np.abs(mat) < np.finfo(np.float).eps] = 0
    return mat
