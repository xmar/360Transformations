from . import Constants
from .Layout import Layout

class CubeMapLayout(Layout):
    def __init__(self, layoutName, ypr, refWidth = None, refHeight = None):
        super().__init__(layoutName, refWidth, refHeight)
        self.yaw = ypr[0]
        self.pitch = ypr[1]
        self.roll = ypr[2]

    def GenerateLayout(self, mediumRatio):
        c = '[{}]\ntype=cubeMap2\n'.format(self.GetName())
        c += super().GetRefResolutionLine()
        c += 'relativeResolution=true\n'

        c+='yaw={}\npitch={}\nroll={}\n'.format(self.yaw, self.pitch, self.roll)
        c+='cubeEdgeLengthFront={}\n'.format(Constants.GOOD_QUALITY)
        for f in ['Left','Right','Top','Bottom']:
            c+='cubeEdgeLength{}={}\n'.format(f, mediumRatio)
        c+='cubeEdgeLengthBack={}\n'.format(Constants.BAD_QUALITY) 
        return c

class CubeMapLayoutCompact(Layout):
    def __init__(self, layoutName, ypr, refWidth = None, refHeight = None):
        super().__init__(layoutName, refWidth, refHeight)
        self.yaw = ypr[0]
        self.pitch = ypr[1]
        self.roll = ypr[2]

    def GenerateLayout(self, mediumRatio):
        c = '[{}]\ntype=cubeMap\n'.format(self.GetName())
        c += super().GetRefResolutionLine()
        c += 'relativeResolution=true\n'

        c+='yaw={}\npitch={}\nroll={}\n'.format(self.yaw, self.pitch, self.roll)
        c+='cubeEdgeLengthFront={}\n'.format(Constants.GOOD_QUALITY)
        for f in ['Left','Right','Top','Bottom']:
            c+='cubeEdgeLength{}={}\n'.format(f, mediumRatio)
        c+='cubeEdgeLengthBack={}\n'.format(Constants.BAD_QUALITY) 
        return c
