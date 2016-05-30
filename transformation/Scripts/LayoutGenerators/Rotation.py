import math
import numpy as np
from numpy import linalg as LA
import copy

class Rotation(object):
    def __init__(self, yaw, pitch, roll):
        '''Create a rotation representation.
        The yaw, pitch and roll angle should be given in radian'''
        self.yaw = yaw
        self.pitch = pitch
        self.roll = roll
        self.rotMat = None

    def __eq__(self, rot):
        return (self.yaw == rot.yaw) and\
            (self.pitch == rot.pitch) and\
            (self.roll == rot.roll)

    def __ne__(self, rot):
        return not self.__eq__(rot)

    def GetEulerAngles(self):
        return (self.yaw, self.pitch, self.roll)

    def GetStrId(self):
        (t,p) = self.ToSpherical()
        return "{0:.2f}_{1:.2f}".format(t,p).replace('.','_').replace('-','m')

    def SetOffset(self, yaw, pitch, roll, inDegrees=False):
        self.yaw += yaw if not inDegrees else math.radians(yaw)
        self.pitch += pitch if not inDegrees else math.radians(pitch)
        self.roll += roll if not inDegrees else math.radians(roll)
        self.rotMat = None

    @classmethod
    def RotationFromSpherical(cls, theta, phi):
        '''Theta and phi are given in degrees'''
        return cls(math.radians(theta), math.radians(phi-90), 0)

    @classmethod
    def RotationFromPosition(cls, x, y, z, rot = None,
                            transpose=False):
        if rot is None:
            rot = cls(0,0,0)
        if transpose:
            p = np.transpose(rot.GetRotMat())*np.matrix('{};{};{}'.format(x,y,z))
        else:
            p = rot.GetRotMat()*np.matrix('{};{};{}'.format(x,y,z))
        (rx, ry, rz) = (p.item(0), p.item(1), p.item(2))
        r = LA.norm(p)
        theta = math.degrees(math.atan2(ry,rx))
        phi = math.degrees(math.acos( rz / r ))
        return cls.RotationFromSpherical(theta, phi)


    @staticmethod
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

    def GetRotMat(self):
        if self.rotMat is None:
            self.rotMat = self.ToRotMat(self.yaw, self.pitch, self.roll)
        return copy.deepcopy(self.rotMat)

    def ToUnitVect(self):
        return self.GetRotMat()*np.matrix('1;0;0')

    def ToSpherical(self):
        '''Return theta and phi in degrees'''
        return (math.degrees(self.yaw), math.degrees(self.pitch) + 90)
