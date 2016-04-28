from .Layout import Layout

class EquirectangularLayout(Layout):
    def __init__(self, layoutName):
        super().__init__(layoutName)


    def GenerateLayout(self, a=None):
        c = '[{}]\ntype=equirectangular\n'.format(self.GetName())
        if a is None:
            a = 1
        c += 'relativeResolution=true\nwidth={0}\nheight={0}\n'.format(a)
        return c
