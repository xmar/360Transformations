from . import Constants
from .Layout import Layout

class PyramidLayout(Layout):
    def __init__(self, layoutName, ypr, base, refWidth = None, refHeight = None):
        super().__init__(layoutName, refWidth, refHeight)
        self.yaw = ypr[0]
        self.pitch = ypr[1]
        self.roll = ypr[2]
        self.base = base

    def GenerateLayout(self, mediumRatio):
        c = '[{}]\ntype=pyramid2\n'.format(self.GetName())
        c += super().GetRefResolutionLine()
        c += 'relativeResolution=true\n'

        c+='pyramidBaseEdge={}\n'.format(self.base)
        c+='yaw={}\npitch={}\nroll={}\n'.format(self.yaw, self.pitch, self.roll)
        c+='pyramidBaseEdgeLength={}\n'.format(Constants.GOOD_QUALITY)
        for f in ['Left','Right','Top','Bottom']:
            c+='pyramidHeight{}={}\n'.format(f, mediumRatio)
        return c
