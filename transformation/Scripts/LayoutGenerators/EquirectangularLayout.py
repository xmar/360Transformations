from .Layout import Layout

class EquirectangularLayout(Layout):
    def __init__(self, layoutName):
        super().__init__(layoutName)


    def GenerateLayout(self, a=None):
        c = '[{}]\ntype=equirectangular\n'.format(self.GetName())
        return c
