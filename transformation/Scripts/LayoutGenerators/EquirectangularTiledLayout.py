from . import Constants
from .Layout import Layout

class EquirectangularTiledLayout(Layout):
    def __init__(self, layoutName, qec):
        super().__init__(layoutName)
        self.qec = qec

    def GenerateLayout(self, a=None):
        c = '[{}]\ntype=equirectangularTiled\n'.format(self.GetName())
        c += 'relativeResolution=true\n'
        (y, p, r) = self.qec.GetEulerAngles() if self.qec is not None else (0, 0, 0)
        c+='yaw={}\npitch={}\nroll={}\n'.format(y-45, p+22.5, r)
        if self.qec is not None:
            if a is None:
                a = Constants.MEDIUM_QUALITY
            for i in range(0,8):
                for j in range (0,8):
                    nbTileDistance = self.qec.DistanceInTileNb(i,j)
                    c += 'equirectangularTile_{}_{}={}\n'.format(i,j, Constants.GOOD_QUALITY if nbTileDistance < 2 else ( a if nbTileDistance < 4 else  Constants.BAD_QUALITY))
        else:
            if a is None:
                a = Constants.MEDIUM_QUALITY
            for i in range(0,8):
                for j in range (0,8):
                    c += 'equirectangularTile_{}_{}={}\n'.format(i,j,a)
        return c
