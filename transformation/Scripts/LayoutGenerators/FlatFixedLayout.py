from .Layout import Layout
from .QEC import QEC
import random
import numpy as np
import math
from .Rotation import Rotation

class FlatFixedLayout(Layout):
    def __init__(self, layoutName, width, height, horizontalAngle, rotation):
        super().__init__(layoutName, rotation=rotation)
        self.width = width
        self.height = height
        self.horizontalAngle = horizontalAngle

    def GenerateLayout(self, mediumRatio):
        c = '[{}]\ntype=flatFixed\n'.format(self.GetName())
        c += 'relativeResolution=false\n'
        c += 'dynamicPositions=false\n'

        c+=super().GetYawPitchRoll()
        c += 'width={}\n'.format(self.width)
        c += 'height={}\n'.format(self.height)
        c += 'horizontalAngleOfVision={}\n'.format(self.horizontalAngle)
        c += 'verticalAngleOfVision=90\n'
        return c

    wasRandomInit = False
    @classmethod
    def GetRandomCenter(cls):
        if not cls.wasRandomInit:
            cls.wasRandomInit = True
            random.seed(1)
        return Rotation.RotationFromSpherical(random.uniform(-180,180), random.uniform(0,180))

    @classmethod
    def SetRandomSeed(cls, seed):
        random.seed(seed)

    @classmethod
    def GetRandomCenterAtDistance(cls, qec, dist):
        '''Return a random point (theta, phi) at the distance dist from the given qec'''
        theta = math.radians(random.uniform(-180,180))
        c = np.matrix('{};{};{}'.format(math.sin(dist)*math.cos(theta), math.sin(dist)*math.sin(theta), math.cos(dist)))
        qecRotation = qec.rotation.GetRotMat()
        toXRotation = Rotation.ToRotMat(0,math.radians(90),0)
        afterFirstRotation = toXRotation*c
        afterFirstRotation[np.abs(afterFirstRotation) < np.finfo(np.float).eps] = 0
        realPoint = np.reshape(qecRotation*afterFirstRotation, 3)
        (rx, ry, rz) = (realPoint.item(0), realPoint.item(1), realPoint.item(2))
        return Rotation.RotationFromPosition(rx, ry, rz)
