from . import Constants
from .Layout import Layout

def GetFaceRes(front, left, right, top, bottom, back):
    c='cubeEdgeLengthFront={}\n'.format(front)
    c+='cubeEdgeLengthLeft={}\n'.format(left)
    c+='cubeEdgeLengthRight={}\n'.format(right)
    c+='cubeEdgeLengthTop={}\n'.format(top)
    c+='cubeEdgeLengthBottom={}\n'.format(bottom)
    c+='cubeEdgeLengthBack={}\n'.format(back)

def GetFaceBitrate(front, left, right, top, bottom, back):
    c='bitrateFront={}\n'.format(front)
    c+='bitrateLeft={}\n'.format(left)
    c+='bitrateRight={}\n'.format(right)
    c+='bitrateTop={}\n'.format(top)
    c+='bitrateBottom={}\n'.format(bottom)
    c+='bitrateBack={}\n'.format(back)

class CubeMapLayout(Layout):
    def __init__(self, layoutName, rotation, refWidth = None, refHeight = None):
        super().__init__(layoutName, refWidth, refHeight, rotation=rotation)

    def GenerateLayout(self, mediumRatio):
        c = '[{}]\ntype=cubeMap2\n'.format(self.GetName())
        c += super().GetRefResolutionLine()
        c += 'relativeResolution=true\n'

        c+=super().GetYawPitchRoll()
        GetFaceRes(Constants.GOOD_QUALITY, Constants.GOOD_QUALITY, Constants.GOOD_QUALITY, Constants.GOOD_QUALITY, Constants.GOOD_QUALITY, Constants.GOOD_QUALITY)
        GetFaceBitrate(Constants.GOOD_QUALITY, mediumRatio, mediumRatio, mediumRatio, mediumRatio, Constants.BAD_QUALITY)
        return c

class CubeMapHigherQualityLayout(Layout):
    def  __init__(self, layoutName, rotation, refWidth = None, refHeight = None):
        super().__init__(layoutName, refWidth, refHeight, rotation=rotation)

    def GenerateLayout(self, mediumRatio):
        c = '[{}]\ntype=cubeMap2\n'.format(self.GetName())
        c += super().GetRefResolutionLine()
        c += 'relativeResolution=true\n'

        c+=super().GetYawPitchRoll()
        GetFaceRes(Constants.GOOD_QUALITY, Constants.GOOD_QUALITY, Constants.GOOD_QUALITY, Constants.GOOD_QUALITY, Constants.GOOD_QUALITY, Constants.GOOD_QUALITY)
        GetFaceBitrate(Constants.GOOD_QUALITY, Constants.GOOD_QUALITY, Constants.GOOD_QUALITY, Constants.GOOD_QUALITY, Constants.GOOD_QUALITY, Constants.BAD_QUALITY)
        return c

class CubeMapLowerQualityLayout(Layout):
    def  __init__(self, layoutName, rotation, refWidth = None, refHeight = None):
        super().__init__(layoutName, refWidth, refHeight, rotation=rotation)

    def GenerateLayout(self, mediumRatio):
        c = '[{}]\ntype=cubeMap2\n'.format(self.GetName())
        c += super().GetRefResolutionLine()
        c += 'relativeResolution=true\n'

        c+=super().GetYawPitchRoll()
        GetFaceRes(Constants.GOOD_QUALITY, Constants.GOOD_QUALITY, Constants.GOOD_QUALITY, Constants.GOOD_QUALITY, Constants.GOOD_QUALITY, Constants.GOOD_QUALITY)
        GetFaceBitrate(Constants.GOOD_QUALITY, Constants.BAD_QUALITY, Constants.BAD_QUALITY, Constants.BAD_QUALITY, Constants.BAD_QUALITY, Constants.BAD_QUALITY)
        return c

class CubeMapMediumQualityLayout(Layout):
    def __init__(self, layoutName, rotation, refWidth = None, refHeight = None):
        super().__init__(layoutName, refWidth, refHeight, rotation=rotation)

    def GenerateLayout(self, mediumRatio):
        c = '[{}]\ntype=cubeMap2\n'.format(self.GetName())
        c += super().GetRefResolutionLine()
        c += 'relativeResolution=true\n'

        c+=super().GetYawPitchRoll()
        GetFaceRes(Constants.GOOD_QUALITY, Constants.GOOD_QUALITY, Constants.GOOD_QUALITY, Constants.GOOD_QUALITY, Constants.GOOD_QUALITY, Constants.GOOD_QUALITY)
        GetFaceBitrate(Constants.GOOD_QUALITY, mediumRatio, mediumRatio, mediumRatio, mediumRatio, mediumRatio)
        return c

class CubeMapLayoutCompact(Layout):
    def __init__(self, layoutName, rotation, refWidth = None, refHeight = None):
        super().__init__(layoutName, refWidth, refHeight, rotation=rotation)

    def GenerateLayout(self, mediumRatio):
        c = '[{}]\ntype=cubeMap\n'.format(self.GetName())
        c += super().GetRefResolutionLine()
        c += 'relativeResolution=true\n'

        c+=super().GetYawPitchRoll()
        GetFaceRes(Constants.GOOD_QUALITY, Constants.GOOD_QUALITY, Constants.GOOD_QUALITY, Constants.GOOD_QUALITY, Constants.GOOD_QUALITY, Constants.GOOD_QUALITY)
        GetFaceBitrate(Constants.GOOD_QUALITY, mediumRatio, mediumRatio, mediumRatio, mediumRatio, Constants.BAD_QUALITY)
        return c
