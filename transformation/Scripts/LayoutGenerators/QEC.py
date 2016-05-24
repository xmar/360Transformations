import os
import math
import numpy as np

class QEC:
    def __init__(self, xTiledCoordinated, yTiledCoordinated, yaw, pitch, roll):
        self.x = xTiledCoordinated
        self.y = yTiledCoordinated
        self.pitch = pitch
        self.yaw = yaw
        self.roll = roll

    def __eq__(self, qec):
        return (self.yaw == qec.yaw) and (self.pitch == qec.pitch) and (self.roll == qec.roll)

    def __ne__(self, qec):
        return not self.__eq__(qec)

    def GetEulerAngles(self):
#        if self.pitch is None or \
#                self.yaw is None or \
#                self.roll is None:
#            self.yaw = self.x*45-157.5
#            self.pitch = -78.75+22.5*self.y
#            self.roll = 0
        return (self.yaw, self.pitch, self.roll)

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
        return "{0:.2f}_{1:.2f}".format(self.yaw,self.pitch).replace('.','_').replace('-','m')


    def ComputeDistance(self, yaw,pitch):
        yaw = math.radians(yaw)
        pitch = math.radians(pitch)
        v1 = np.reshape(self.ToVect(),3)
        v2 = np.reshape(self.ToRotMat(yaw,pitch,0)*np.matrix('1;0;0'), 3)
        #print ('v1=',v1,'\nv2=', v2, '\ninner =', np.inner(v1,v2)) 
        return math.acos(min(1,max(-1,np.inner(v1,v2))))

    @staticmethod
    def ComputeDistance2( point1, point2 ):
        (y1,p1) = point1
        (y2,p2) = point2
        y1 = math.radians(y1)
        y2 = math.radians(y2)
        p1 = math.radians(p1)
        p2 = math.radians(p2)
        v1 = np.reshape(self.ToRotMat(y1,p1,0)*np.matrix('1;0;0'),3)
        v2 = np.reshape(self.ToRotMat(y2,p2,0)*np.matrix('1;0;0'), 3)
        return math.acos(np.inner(v1,v2))


    @staticmethod
    def TestQecGenerator(nbQec = 16):
        if os.path.exists('LayoutGenerators/QecCenters/{}.txt'.format(nbQec)):
            with open('LayoutGenerators/QecCenters/{}.txt'.format(nbQec), 'r') as i:
                first = True
                for line in i:
                    (Id, x, y, z) = map(float,line.replace(':','').split('\t'))
                    p = np.matrix('{};{};{}'.format(x,y,z))
                    if first:
                        first = False
                        theta = math.atan2(y,x)
                        phi = math.acos( z ) - math.pi/2
                        rotMat = np.transpose(QEC.ToRotMat(theta, phi, 0))
                    newP = rotMat*p
                    newP[np.abs(newP) < np.finfo(np.float).eps] = 0
                    (rx, ry, rz) = (newP.item(0), newP.item(1), newP.item(2))
                    theta = math.degrees(math.atan2(ry,rx))
                    phi = math.degrees(math.acos( rz ))
                    yield QEC(4,3, theta, phi, 0)
        else:
            raise NotImplementedError

    @classmethod
    def GetClosestQecFromTestQec(cls, yaw, pitch, nbQec = 16):
        qec = None
        distance = 5
        for q in cls.TestQecGenerator(nbQec):
            d = q.ComputeDistance(yaw,pitch)
            #if distance is None or d < distance:
            if d < distance:
                distance = d
                qec = q
        return qec

    @staticmethod
    def ToRotMat(yaw, pitch, roll):
        '''yaw, pitch and roll in radian'''
        cosP = math.cos(pitch)
        sinP = math.sin(pitch)
        cosY = math.cos(yaw)
        sinY = math.sin(yaw)
        cosR = math.cos(roll)
        sinR = math.sin(roll)
        return np.matrix('{},{},{};{},{},{};{},{},{}'.format(
                cosP * cosY,     cosY*sinP*sinR -sinY*cosR,  cosY*sinP*cosR + sinY * sinR,
                sinY*cosP,       cosY * cosR,                sinY*sinP*cosR - sinR * cosY,
                -sinP,           cosP * sinR,                cosP * cosR))

    def GetRotMat(self):
        return self.ToRotMat(math.radians(self.yaw), math.radians(self.pitch), math.radians(self.roll))

    def ToVect(self):
        return self.GetRotMat()*np.matrix('1;0;0')

