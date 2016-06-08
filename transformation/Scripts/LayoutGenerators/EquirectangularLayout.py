from .Layout import Layout

class EquirectangularLayout(Layout):
    def __init__(self, layoutName, refWidth = None, refHeight = None):
        super().__init__(layoutName, refWidth, refHeight)


    def GenerateLayout(self, a=None):
        c = '[{}]\ntype=equirectangular\n'.format(self.GetName())
        c += self.GetRefResolutionLine()
        c+='yaw={}\npitch={}\nroll={}\n'.format(0, 0, 0)
        if a is None:
            a = 1
        c += 'relativeResolution=true\nwidth={0}\nheight={0}\n'.format(a)
        c += 'bitrate={}'.format(a)
        return c
