from . import Constants
from .Layout import Layout
import copy

class RhombicDodecaLayout(Layout):
    def __init__(self, layoutName, rotation, refWidth = None, refHeight = None):
        rotation = copy.deepcopy(rotation)
        rotation.SetOffset(19.4712, -26.5650, 0, inDegrees=True)
        super().__init__(layoutName, refWidth, refHeight, rotation=rotation)

    def GenerateLayout(self, mediumRatio):
        c = '[{}]\ntype=rhombicDodeca\n'.format(self.GetName())
        c += super().GetRefResolutionLine()
        c += 'relativeResolution=true\n'

        c+=super().GetYawPitchRoll()
        for i in [1,2,3,4,5,6,7,8,9,10,11,12]:
            c+='rhombEdgeLengthFace{}={}\n'.format(i, Constants.GOOD_QUALITY)

        for i in [1,5]:
            c+='rhombFace{}Bitrate={}\n'.format(i, Constants.GOOD_QUALITY)
        for i in [2,3,4,6,7,8,10,11]:
            c+='rhombFace{}Bitrate={}\n'.format(i, mediumRatio)
        for i in [9,12]:
            c+='rhombFace{}Bitrate={}\n'.format(i, Constants.BAD_QUALITY)
        return c

class RhombicDodecaHigherQualityLayout(Layout):
    def __init__(self, layoutName, rotation, refWidth = None, refHeight = None):
        rotation = copy.deepcopy(rotation)
        rotation.SetOffset(96.512194, -133.458231, 0, inDegrees=True)
        super().__init__(layoutName, refWidth, refHeight, rotation=rotation)

    def GenerateLayout(self, mediumRatio):
        c = '[{}]\ntype=rhombicDodeca\n'.format(self.GetName())
        c += super().GetRefResolutionLine()
        c += 'relativeResolution=true\n'

        c+=super().GetYawPitchRoll()
        for i in [1,2,3,4,5,6,7,8,9,10,11,12]:
            c+='rhombEdgeLengthFace{}={}\n'.format(i, Constants.GOOD_QUALITY)

        for i in [1,5, 4]:
            c+='rhombFace{}Bitrate={}\n'.format(i, Constants.GOOD_QUALITY)
        for i in [2,3,6,7,8,9,10,11,12]:
            c+='rhombFace{}Bitrate={}\n'.format(i, mediumRatio)
        return c

class RhombicDodecaMediumQualityLayout(Layout):
    def __init__(self, layoutName, rotation, refWidth = None, refHeight = None):
        rotation = copy.deepcopy(rotation)
        rotation.SetOffset(96.512194, -133.458231, 0, inDegrees=True)
        super().__init__(layoutName, refWidth, refHeight, rotation=rotation)

    def GenerateLayout(self, mediumRatio):
        c = '[{}]\ntype=rhombicDodeca\n'.format(self.GetName())
        c += super().GetRefResolutionLine()
        c += 'relativeResolution=true\n'

        c+=super().GetYawPitchRoll()
        for i in [1,2,3,4,5,6,7,8,9,10,11,12]:
            c+='rhombEdgeLengthFace{}={}\n'.format(i, Constants.GOOD_QUALITY)

        for i in [1,5,4]:
            c+='rhombFace{}Bitrate={}\n'.format(i, Constants.GOOD_QUALITY)
        for i in [2,3,6,7,10,11]:
            c+='rhombFace{}Bitrate={}\n'.format(i, mediumRatio)
        for i in [8,9,12]:
            c+='rhombFace{}Bitrate={}\n'.format(i, Constants.BAD_QUALITY)
        return c

class RhombicDodecaEqualQualityLayout(Layout):
    def __init__(self, layoutName, rotation, refWidth = None, refHeight = None):
        rotation = copy.deepcopy(rotation)
        super().__init__(layoutName, refWidth, refHeight, rotation=rotation)

    def GenerateLayout(self, mediumRatio):
        c = '[{}]\ntype=rhombicDodeca\n'.format(self.GetName())
        c += super().GetRefResolutionLine()
        c += 'relativeResolution=true\n'

        c+=super().GetYawPitchRoll()
        for i in [1,2,3,4,5,6,7,8,9,10,11,12]:
            c+='rhombEdgeLengthFace{}={}\n'.format(i, Constants.GOOD_QUALITY)

        for i in [1,2,3,4,5]:
            c+='rhombFace{}Bitrate={}\n'.format(i, Constants.GOOD_QUALITY)
        for i in [6,7,8,9,10,11,12]:
            c+='rhombFace{}Bitrate={}\n'.format(i, mediumRatio)
        return c
