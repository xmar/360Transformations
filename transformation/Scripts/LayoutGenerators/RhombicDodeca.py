from . import Constants
from .Layout import Layout

class RhombicDodecaLayout(Layout):
    def __init__(self, layoutName, ypr):
        super().__init__(layoutName)
        self.yaw = ypr[0] +19.4712 #+ 35.2643 #- 19.4712
        self.pitch = ypr[1] - 26.5650
        self.roll = ypr[2]

    def GenerateLayout(self, mediumRatio):
        c = '[{}]\ntype=rhombicDodeca\n'.format(self.GetName())
        c += 'relativeResolution=true\n'

        c+='yaw={}\npitch={}\nroll={}\n'.format(self.yaw, self.pitch, self.roll)
        for i in [1,5]: #TODO check the right face ID
            c+='rhombEdgeLengthFace{}={}\n'.format(i, Constants.GOOD_QUALITY)
        for i in [2,3,4,6,7]:
            c+='rhombEdgeLengthFace{}={}\n'.format(i, mediumRatio)
        for i in [8,9,10,11,12]:
            c+='rhombEdgeLengthFace{}={}\n'.format(i, Constants.BAD_QUALITY)
        return c
