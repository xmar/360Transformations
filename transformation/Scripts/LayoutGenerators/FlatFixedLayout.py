from .Layout import Layout
import random

class FlatFixedLayout(Layout):
    def __init__(self, layoutName, width, height, horizontalAngle, ypr):
        super().__init__(layoutName)
        self.yaw = ypr[0]
        self.pitch = ypr[1]
        self.roll = ypr[2]
        self.width = width
        self.height = height
        self.horizontalAngle = horizontalAngle

    def GenerateLayout(self, mediumRatio):
        c = '[{}]\ntype=flatFixed\n'.format(self.GetName())
        c += 'relativeResolution=false\n'

        c+='yaw={}\npitch={}\nroll={}\n'.format(self.yaw, self.pitch, self.roll)
        c += 'width={}\n'.format(self.width)
        c += 'height={}\n'.format(self.height)
        c += 'horizontalAngleOfVision={}\n'.format(self.horizontalAngle)
        return c
    
    wasRandomInit = False
    @classmethod
    def GetRandomCenter(cls):
        if not cls.wasRandomInit:
            cls.wasRandomInit = True
            random.seed(1)
        return (random.uniform(-180,180), random.uniform(-90,90))

