from . import Constants
from .Layout import Layout

class RhombicDodecaLayout(Layout):
    def __init__(self, layoutName, ypr, refWidth = None, refHeight = None):
        super().__init__(layoutName, refWidth, refHeight)
        self.yaw = ypr[0] +19.4712 #+ 35.2643 #- 19.4712
        self.pitch = ypr[1] - 26.5650
        self.roll = ypr[2]

    def GenerateLayout(self, mediumRatio):
        c = '[{}]\ntype=rhombicDodeca\n'.format(self.GetName())
        c += super().GetRefResolutionLine()
        c += 'relativeResolution=true\n'

        c+='yaw={}\npitch={}\nroll={}\n'.format(self.yaw, self.pitch, self.roll)
        for i in [1,5]:
            c+='rhombEdgeLengthFace{}={}\n'.format(i, Constants.GOOD_QUALITY)
        for i in [2,3,4,6,7,8,10,11]:
            c+='rhombEdgeLengthFace{}={}\n'.format(i, mediumRatio)
        for i in [9,12]:
            c+='rhombEdgeLengthFace{}={}\n'.format(i, Constants.BAD_QUALITY)
        return c

class RhombicDodecaHigherQualityLayout(Layout):
    def __init__(self, layoutName, ypr, refWidth = None, refHeight = None):
        super().__init__(layoutName, refWidth, refHeight)
        self.yaw = ypr[0] + 96.512194  #+ 35.2643 #- 19.4712
        self.pitch = ypr[1] -133.458231
        self.roll = ypr[2]

    def GenerateLayout(self, mediumRatio):
        c = '[{}]\ntype=rhombicDodeca\n'.format(self.GetName())
        c += super().GetRefResolutionLine()
        c += 'relativeResolution=true\n'

        c+='yaw={}\npitch={}\nroll={}\n'.format(self.yaw, self.pitch, self.roll)
        for i in [1,5,4]:
            c+='rhombEdgeLengthFace{}={}\n'.format(i, Constants.GOOD_QUALITY)
        for i in [2,3,6,7,8,9,10,11,12]:
            c+='rhombEdgeLengthFace{}={}\n'.format(i, mediumRatio)
        return c

class RhombicDodecaMediumQualityLayout(Layout):
    def __init__(self, layoutName, ypr, refWidth = None, refHeight = None):
        super().__init__(layoutName, refWidth, refHeight)
        self.yaw = ypr[0] + 96.512194 #+ 35.2643 #- 19.4712
        self.pitch = ypr[1] -133.458231
        self.roll = ypr[2]

    def GenerateLayout(self, mediumRatio):
        c = '[{}]\ntype=rhombicDodeca\n'.format(self.GetName())
        c += super().GetRefResolutionLine()
        c += 'relativeResolution=true\n'

        c+='yaw={}\npitch={}\nroll={}\n'.format(self.yaw, self.pitch, self.roll)
        for i in [1,5,4]:
            c+='rhombEdgeLengthFace{}={}\n'.format(i, Constants.GOOD_QUALITY)
        for i in [2,3,6,7,10,11]:
            c+='rhombEdgeLengthFace{}={}\n'.format(i, mediumRatio)
	for i in [8,9,12]:
	    c+='rhombEdgeLengthFace{}={}\n'.format(i, Constants.BAD_QUALITY)
        return c

class RhombicDodecaEqualQualityLayout(Layout):
    def __init__(self, layoutName, ypr, refWidth = None, refHeight = None):
        super().__init__(layoutName, refWidth, refHeight)
        self.yaw = ypr[0] 
        self.pitch = ypr[1]
        self.roll = ypr[2]

    def GenerateLayout(self, mediumRatio):
        c = '[{}]\ntype=rhombicDodeca\n'.format(self.GetName())
        c += super().GetRefResolutionLine()
        c += 'relativeResolution=true\n'

        c+='yaw={}\npitch={}\nroll={}\n'.format(self.yaw, self.pitch, self.roll)
        for i in [1,2,3,4,5]:
            c+='rhombEdgeLengthFace{}={}\n'.format(i, Constants.GOOD_QUALITY)
        for i in [6,7,8,9,10,11,12]:
            c+='rhombEdgeLengthFace{}={}\n'.format(i, mediumRatio)
        return c



