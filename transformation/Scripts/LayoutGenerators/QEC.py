import math

class QEC:
    def __init__(self, xTiledCoordinated, yTiledCoordinated):
        self.x = xTiledCoordinated
        self.y = yTiledCoordinated
        self.pitch = None
        self.yaw = None
        self.roll = None

    def GetEulerAngles(self):
        if self.pitch is None or \
                self.yaw is None or \
                self.roll is None:
            self.yaw = self.x*45-157.5
            self.pitch = -78.75+22.5*self.y
            self.roll = 0
        return (self.yaw, self.pitch, self.roll)

    def DistanceInTileNb(self, i, j):
        #Normalize i value
        ibis = i - 8
        iters = i + 8
        di = abs(i-self.x)
        dibis = abs(ibis-self.x)
        diters = abs(iters-self.x)
        minD = min(di,dibis,diters)
        return max(minD, abs(j-self.y))

    def GetTileCoordinate(self):
        return (self.x,self.y)

    def ComputeDistance(self, yaw,pitch):
        (y,p,r) = self.GetEulerAngles()
        return math.sqrt(math.pow(min(abs(y-yaw), abs(y-360-yaw), abs(y+360-yaw)),2)+math.pow(p-pitch,2))

    @staticmethod
    def TestQecGenerator():
        for i in [1,3,5,7]:
            for j in [1,3,4,6]:
                yield QEC(i,j)

    @classmethod
    def GetClosestQecFromTestQec(cls, yaw,pitch):
        qec = None
        distance = None
        for q in cls.TestQecGenerator():
            d = q.ComputeDistance(yaw,pitch)
            if distance is None or d < distance:
                distance = d
                qec = q
        return qec
