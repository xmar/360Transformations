from . import Constants
from .Layout import Layout

class CubeMapLayout(Layout):
    def __init__(self, layoutName, rotation, refWidth = None, refHeight = None):
        super().__init__(layoutName, refWidth, refHeight, rotation=rotation)

    def GenerateLayout(self, mediumRatio):
        c = '[{}]\ntype=cubeMap2\n'.format(self.GetName())
        c += super().GetRefResolutionLine()
        c += 'relativeResolution=true\n'

        c+=super().GetYawPitchRoll()
        c+='cubeEdgeLengthFront={}\n'.format(Constants.GOOD_QUALITY)
        for f in ['Left','Right','Top','Bottom']:
            c+='cubeEdgeLength{}={}\n'.format(f, mediumRatio)
        c+='cubeEdgeLengthBack={}\n'.format(Constants.BAD_QUALITY)
        return c

class CubeMapHigherQualityLayout(Layout):
    def  __init__(self, layoutName, rotation, refWidth = None, refHeight = None):
        super().__init__(layoutName, refWidth, refHeight, rotation=rotation)

    def GenerateLayout(self, mediumRatio):
        c = '[{}]\ntype=cubeMap2\n'.format(self.GetName())
        c += super().GetRefResolutionLine()
        c += 'relativeResolution=true\n'

        c+=super().GetYawPitchRoll()
        c+='cubeEdgeLengthFront={}\n'.format(Constants.GOOD_QUALITY)
        c+='cubeEdgeLengthLeft={}\n'.format(Constants.GOOD_QUALITY)
        c+='cubeEdgeLengthRight={}\n'.format(Constants.GOOD_QUALITY)
        c+='cubeEdgeLengthTop={}\n'.format(Constants.GOOD_QUALITY)
        c+='cubeEdgeLengthBottom={}\n'.format(Constants.GOOD_QUALITY)
        c+='cubeEdgeLengthBack={}\n'.format(Constants.BAD_QUALITY)
        return c

class CubeMapLowerQualityLayout(Layout):
    def  __init__(self, layoutName, rotation, refWidth = None, refHeight = None):
        super().__init__(layoutName, refWidth, refHeight, rotation=rotation)

    def GenerateLayout(self, mediumRatio):
        c = '[{}]\ntype=cubeMap2\n'.format(self.GetName())
        c += super().GetRefResolutionLine()
        c += 'relativeResolution=true\n'

        c+=super().GetYawPitchRoll()
        c+='cubeEdgeLengthFront={}\n'.format(Constants.GOOD_QUALITY)
        c+='cubeEdgeLengthLeft={}\n'.format(Constants.BAD_QUALITY)
        c+='cubeEdgeLengthRight={}\n'.format(Constants.BAD_QUALITY)
        c+='cubeEdgeLengthTop={}\n'.format(Constants.BAD_QUALITY)
        c+='cubeEdgeLengthBottom={}\n'.format(Constants.BAD_QUALITY)
        c+='cubeEdgeLengthBack={}\n'.format(Constants.BAD_QUALITY)
        return c

class CubeMapMediumQualityLayout(Layout):
    def __init__(self, layoutName, rotation, refWidth = None, refHeight = None):
        super().__init__(layoutName, refWidth, refHeight, rotation=rotation)

    def GenerateLayout(self, mediumRatio):
        c = '[{}]\ntype=cubeMap2\n'.format(self.GetName())
        c += super().GetRefResolutionLine()
        c += 'relativeResolution=true\n'

        c+=super().GetYawPitchRoll()
        c+='cubeEdgeLengthFront={}\n'.format(Constants.GOOD_QUALITY)
        for f in ['Left','Right','Top','Bottom','Back']:
            c+='cubeEdgeLength{}={}\n'.format(f, mediumRatio)
        return c

class CubeMapLayoutCompact(Layout):
    def __init__(self, layoutName, rotation, refWidth = None, refHeight = None):
        super().__init__(layoutName, refWidth, refHeight, rotation=rotation)

    def GenerateLayout(self, mediumRatio):
        c = '[{}]\ntype=cubeMap\n'.format(self.GetName())
        c += super().GetRefResolutionLine()
        c += 'relativeResolution=true\n'

        c+=super().GetYawPitchRoll()
        c+='cubeEdgeLengthFront={}\n'.format(Constants.GOOD_QUALITY)
        for f in ['Left','Right','Top','Bottom']:
            c+='cubeEdgeLength{}={}\n'.format(f, mediumRatio)
        c+='cubeEdgeLengthBack={}\n'.format(Constants.BAD_QUALITY)
        return c
