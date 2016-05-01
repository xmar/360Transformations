import math

class QEC:
    def __init__(self, xTiledCoordinated, yTiledCoordinated, yaw, pitch, roll):
        self.x = xTiledCoordinated
        self.y = yTiledCoordinated
        self.pitch = pitch
        self.yaw = yaw
        self.roll = roll

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
        return "{0:.2f}_{1:.2f}".format(abs(self.yaw),abs(self.pitch)).replace('.','_')


    def ComputeDistance(self, yaw,pitch):
        (y,p,r) = self.GetEulerAngles()
        yaw = math.radians(yaw)
        pitch = math.radians(pitch)
        y = math.radians(y)
        p = math.radians(p)
        #return math.sqrt(math.pow(min(abs(y-yaw), abs(y-360-yaw), abs(y+360-yaw)),2)+math.pow(p-pitch,2))
        return 2*math.asin(math.sqrt(math.pow(math.sin(abs(p-pitch)/2),2) + math.cos(abs(p))*math.cos(abs(pitch))*math.pow(math.sin(abs(y-yaw)/2),2)))

    @staticmethod
    def ComputeDistance2( point1, point2 ):
        (y1,p1) = point1
        (y2,p2) = point2
        y1 = math.radians(y1)
        y2 = math.radians(y2)
        p1 = math.radians(p1)
        p2 = math.radians(p2)
        return 2*math.asin(math.sqrt(math.pow(math.sin(abs(p1-p2)/2),2) + math.cos(abs(p1))*math.cos(abs(p2))*math.pow(math.sin(abs(y1-y2)/2),2)))

    @staticmethod
    def TestQecGenerator():
        for i in [1,3,5,7]:
            for j in [1,3,4,6]:
                yaw = i*45-157.5
                pitch = -78.75+22.5*j
                roll = 0
                yield QEC(4,3, yaw, pitch, roll)

    @classmethod
    def GetClosestQecFromTestQec(cls, yaw,pitch):
        qec = None
        distance = 5
        for q in cls.TestQecGenerator():
            d = q.ComputeDistance(yaw,pitch)
            print('d =',d," yaw = ",yaw, ' ', q.yaw,"; pitch = ", pitch, ' ', q.pitch)
            #if distance is None or d < distance:
            if d < distance:
                distance = d
                qec = q
        print ('min = ', qec.yaw, " ", qec.pitch, ' ', distance)
        return qec
