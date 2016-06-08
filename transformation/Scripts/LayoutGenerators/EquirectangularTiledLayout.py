from . import Constants
from .Layout import Layout
from .Rotation import Rotation

class EquirectangularTiledLayout(Layout):
    def __init__(self, layoutName, qec, refWidth = None, refHeight = None):
        rotation = qec.GetRotMat() if qec is not None else Rotation(0,0,0)
        rotation.SetOffset(45, -22.5, 0, inDegrees=True)
        super().__init__(layoutName, refWidth, refHeight, rotation=rotation)
        self.qec = qec


    def GenerateLayout(self, a=None):
        c = '[{}]\ntype=equirectangularTiled\n'.format(self.GetName())
        c += super().GetRefResolutionLine()
        c += 'relativeResolution=true\n'
        c+=super().GetYawPitchRoll()
        if self.qec is not None:
            if a is None:
                a = Constants.MEDIUM_QUALITY
            for i in range(0,8):
                for j in range (0,8):
                    nbTileDistance = self.qec.DistanceInTileNb(i,j)
                    c += 'equirectangularTile_{}_{}={}\n'.format(i,j, Constants.GOOD_QUALITY )
                    c += 'equirectangularTileBitrate_{}_{}={}\n'.format(i,j, Constants.GOOD_QUALITY if nbTileDistance < 2 else ( a if nbTileDistance < 4 else  Constants.BAD_QUALITY))

        else:
            if a is None:
                a = Constants.MEDIUM_QUALITY
            for i in range(0,8):
                for j in range (0,8):
                    c += 'equirectangularTile_{}_{}={}\n'.format(i,j, Constants.GOOD_QUALITY)
                    c += 'equirectangularTileBitrate_{}_{}={}\n'.format(i,j,a)

        return c

class EquirectangularTiledHigherQualityLayout(Layout):
    def __init__(self, layoutName, qec, refWidth = None, refHeight = None):
        rotation = qec.GetRotMat()
        rotation.SetOffset(45, -22.5, 0, inDegrees=True)
        super().__init__(layoutName, refWidth, refHeight, rotation=rotation)
        self.qec = qec


    def GenerateLayout(self, a=None):
        c = '[{}]\ntype=equirectangularTiled\n'.format(self.GetName())
        c += super().GetRefResolutionLine()
        c += 'relativeResolution=true\n'
        c+=super().GetYawPitchRoll()
        for i in range(0,8):
            for j in range (0,8):
                nbTileDistance = self.qec.DistanceInTileNb(i,j)
                c += 'equirectangularTile_{}_{}={}\n'.format(i,j, Constants.GOOD_QUALITY )
                c += 'equirectangularTileBitrate_{}_{}={}\n'.format(i,j, Constants.GOOD_QUALITY if nbTileDistance < 2 else ( Constants.HIGHER_MEDIUM_QUALITY if nbTileDistance < 3 else  (Constants.MEDIUM_QUALITY if nbTileDistance < 4 else  Constants.BAD_QUALITY)))
        return c

class EquirectangularTiledLowerQualityLayout(Layout):
    def __init__(self, layoutName, qec, refWidth = None, refHeight = None):
        rotation = qec.GetRotMat()
        rotation.SetOffset(45, -22.5, 0, inDegrees=True)
        super().__init__(layoutName, refWidth, refHeight, rotation=rotation)
        self.qec = qec


    def GenerateLayout(self, a=None):
        c = '[{}]\ntype=equirectangularTiled\n'.format(self.GetName())
        c += super().GetRefResolutionLine()
        c += 'relativeResolution=true\n'
        c+=super().GetYawPitchRoll()
        for i in range(0,8):
            for j in range (0,8):
                nbTileDistance = self.qec.DistanceInTileNb(i,j)
                c += 'equirectangularTile_{}_{}={}\n'.format(i,j, Constants.GOOD_QUALITY)
                c += 'equirectangularTileBitrate_{}_{}={}\n'.format(i,j, Constants.GOOD_QUALITY if nbTileDistance < 3 else Constants.BAD_QUALITY)
        return c

class EquirectangularTiledMediumQualityLayout(Layout):
    def __init__(self, layoutName, qec, refWidth = None, refHeight = None):
        rotation = qec.GetRotMat()
        rotation.SetOffset(45, -22.5, 0, inDegrees=True)
        super().__init__(layoutName, refWidth, refHeight, rotation=rotation)
        self.qec = qec


    def GenerateLayout(self, a=None):
        c = '[{}]\ntype=equirectangularTiled\n'.format(self.GetName())
        c += super().GetRefResolutionLine()
        c += 'relativeResolution=true\n'
        c+=super().GetYawPitchRoll()
        for i in range(0,8):
            for j in range (0,8):
                nbTileDistance = self.qec.DistanceInTileNb(i,j)
                c += 'equirectangularTile_{}_{}={}\n'.format(i,j, Constants.GOOD_QUALITY)
                c += 'equirectangularTileBitrate_{}_{}={}\n'.format(i,j, Constants.GOOD_QUALITY if nbTileDistance < 3 else  Constants.MEDIUM_QUALITY)
        return c
