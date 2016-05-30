import os
import math
import numpy as np
from .Rotation import Rotation
import copy

class QEC:
    def __init__(self, xTiledCoordinated, yTiledCoordinated, rotation):
        self.x = xTiledCoordinated
        self.y = yTiledCoordinated
        self.rotation = copy.deepcopy(rotation)

    def __eq__(self, qec):
        return self.rotation == qec.rotation

    def __ne__(self, qec):
        return not self.__eq__(qec)

    def GetEulerAngles(self):
        return self.rotation.GetEulerAngles()

    def DistanceInTileNb(self, i, j):
        #Normalize i value
        ibis = i - 8
        iters = i + 8
        di = abs(i-self.x)
        dibis = abs(ibis-self.x)
        diters = abs(iters-self.x)
        minD = min(di,dibis,diters)
        dist = max(minD, abs(j-self.y))
        if self.y == 1 or self.y == 2:
            if j <= 5 or (j == 6 and i == self.x):
                return min(2,dist)
        elif self.y ==  5 or self.y == 6:
            if j >= 2 or (j == 1 and i == self.x):
                return min(2,dist)
        return dist

    def GetTileCoordinate(self):
        return (self.x,self.y)

    def GetStrId(self):
        return self.rotation.GetStrId()

    def ComputeDistance(self, rot):
        v1 = np.reshape(self.rotation.ToUnitVect(),3)
        v2 = np.reshape(rot.ToUnitVect(), 3)
        return math.acos(min(1,max(-1,np.inner(v1,v2))))

    @staticmethod
    def ComputeDistance2( rot1, rot2 ):
        v1 = np.reshape(rot1.ToUnitVect(),3)
        v2 = np.reshape(rot2.ToUnitVect(), 3)
        return math.acos(np.inner(v1,v2))


    @staticmethod
    def TestQecGenerator(nbQec = 16):
        if os.path.exists('LayoutGenerators/QecCenters/{}.txt'.format(nbQec)):
            with open('LayoutGenerators/QecCenters/{}.txt'.format(nbQec), 'r') as i:
                first = True
                globalRot = None
                for line in i:
                    (Id, x, y, z) = map(float,line.replace(':','').split('\t'))
                    if globalRot is None:
                        globalRot = Rotation.RotationFromPosition(x, y, z)
                    rot = Rotation.RotationFromPosition(x, y, z,
                            globalRot, transpose=True)
                    yield QEC(4,3, rot)
        else:
            raise NotImplementedError

    @classmethod
    def GetClosestQecFromTestQec(cls, rot, nbQec = 16):
        qec = None
        distance = 5
        for q in cls.TestQecGenerator(nbQec):
            d = q.ComputeDistance(rot)
            #if distance is None or d < distance:
            if d < distance:
                distance = d
                qec = q
        return qec

    def GetRotMat(self):
        return copy.deepcopy(self.rotation)
