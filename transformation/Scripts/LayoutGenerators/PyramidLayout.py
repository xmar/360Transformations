from . import Constants
from .Layout import Layout

class PyramidLayout(Layout):
    def __init__(self, layoutName, rotation, base, refWidth = None, refHeight = None):
        super().__init__(layoutName, refWidth, refHeight, rotation=rotation)
        self.base = base

    def GenerateLayout(self, mediumRatio):
        c = '[{}]\ntype=pyramid2\n'.format(self.GetName())
        c += super().GetRefResolutionLine()
        c += 'relativeResolution=true\n'

        c+='pyramidBaseEdge={}\n'.format(self.base)
        c+=super().GetYawPitchRoll()
        c+='pyramidBaseEdgeLength={}\n'.format(Constants.GOOD_QUALITY)
        for f in ['Left','Right','Top','Bottom']:
            c+='pyramidHeight{}={}\n'.format(f, Constants.GOOD_QUALITY)
        c+='pyramidBaseBitrate={}\n'.format(Constants.GOOD_QUALITY)
        for f in ['Left','Right','Top','Bottom']:
            c+='pyramid{}Bitrate={}\n'.format(f, mediumRatio)
        return c

class PyramidHigherQualityLayout(Layout):
    def __init__(self, layoutName, rotation, base, refWidth = None, refHeight = None):
        super().__init__(layoutName, refWidth, refHeight, rotation=rotation)
        self.base = base

    def GenerateLayout(self, mediumRatio):
        c = '[{}]\ntype=pyramid2\n'.format(self.GetName())
        c += super().GetRefResolutionLine()
        c += 'relativeResolution=true\n'

        c+='pyramidBaseEdge={}\n'.format(self.base)
        c+=super().GetYawPitchRoll()
        c+='pyramidBaseEdgeLength={}\n'.format(Constants.GOOD_QUALITY)
        for f in ['Left','Right','Top','Bottom']:
            c+='pyramidHeight{}={}\n'.format(f, Constants.GOOD_QUALITY)
        c+='pyramidBaseBitrate={}\n'.format(Constants.GOOD_QUALITY)
        for f in ['Left','Right','Top','Bottom']:
            c+='pyramid{}Bitrate={}\n'.format(f, Constants.GOOD_QUALITY)
        return c

class PyramidLowerQualityLayout(Layout):
    def __init__(self, layoutName, rotation, base, refWidth = None, refHeight = None):
        super().__init__(layoutName, refWidth, refHeight, rotation=rotation)
        self.base = base

    def GenerateLayout(self, mediumRatio):
        c = '[{}]\ntype=pyramid2\n'.format(self.GetName())
        c += super().GetRefResolutionLine()
        c += 'relativeResolution=true\n'

        c+='pyramidBaseEdge={}\n'.format(self.base)
        c+=super().GetYawPitchRoll()
        c+='pyramidBaseEdgeLength={}\n'.format(Constants.GOOD_QUALITY)
        for f in ['Left','Right','Top','Bottom']:
            c+='pyramidHeight{}={}\n'.format(f, Constants.GOOD_QUALITY)
        c+='pyramidBaseBitrate={}\n'.format(Constants.GOOD_QUALITY)
        for f in ['Left','Right','Top','Bottom']:
            c+='pyramid{}Bitrate={}\n'.format(f, Constants.BAD_QUALITY)
        return c
