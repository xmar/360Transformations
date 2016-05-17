import numpy as np
import math

class FoVCenters:
    def __init__(self, matrix3x3):
        self.rotMat = np.matrix(matrix3x3)
        i = np.matrix([[1],[0],[0]])
        self.c = np.reshape(self.rotMat*i, 3)
        cx = self.c.item(0)
        cy = self.c.item(1)
        cz = self.c.item(2)
        self.centerPhi = math.acos(cz / math.sqrt( cx**2 + cy**2 + cz**2))
        self.centerTheta = math.atan2(cy,cx)
        self.yaw = math.atan2(self.rotMat.item(1,0), self.rotMat.item(0,0))
        self.pitch = math.atan2(-self.rotMat.item(2,0), math.sqrt(self.rotMat.item(2,1)**2 + self.rotMat.item(2,2)**2))
        self.roll = math.atan2(self.rotMat.item(2,1), self.rotMat.item(2,2))
        #print('yaw = {}, pitch = {}, roll = {} ; theta = {}, phi = {}'.format(self.yaw, self.pitch, self.roll, self.centerTheta, self.centerPhi))

    def OrthodromicDistance(self, fovCenter):
        #print(np.inner(self.c, fovCenter.c))
        return math.acos(min(1, max(-1,np.inner(self.c, fovCenter.c))))

