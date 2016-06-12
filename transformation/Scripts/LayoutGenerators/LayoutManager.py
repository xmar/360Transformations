# from . import CubeMapLayout
# from . import EquirectangularLayout
# from . import EquirectangularTiledLayout
# from . import FlatFixedLayout
# from . import PyramidLayout
# from . import RhombicDodeca
import LayoutGenerators

class LayoutGen(object):
    def __init__(self, name, layoutCls):
        self.name = name
        self.layoutCls = layoutCls

    def GetName(self):
        return self.name

    def GetGeneratorFct(self, qec, width, heigh):
        if self.layoutCls is LayoutGenerators.PyramidLowerQualityLayout or self.layoutCls is LayoutGenerators.PyramidHigherQualityLayout or self.layoutCls is LayoutGenerators.PyramidLayout:
            return lambda n,ypr: self.layoutCls(n, ypr, 2.5, width, heigh)
        if self.layoutCls is LayoutGenerators.EquirectangularTiledLowerQualityLayout or self.layoutCls is LayoutGenerators.EquirectangularTiledMediumQualityLayout or\
         self.layoutCls is LayoutGenerators.EquirectangularTiledHigherQualityLayout or self.layoutCls is LayoutGenerators.EquirectangularTiledLayout:
            return lambda n,ypr: self.layoutCls(n, qec, width, heigh)
        return lambda n,ypr: self.layoutCls(n, ypr, width, heigh)



class LayoutManager(object):
    """This class is used to list the layout that will be tested"""
    def __init__(self):
        self.layoutList = []

    def AddLayout(self, layoutName):
        if 'CubeMapLower' in layoutName:
            layoutCls = LayoutGenerators.CubeMapLowerQualityLayout
        elif 'CubeMapHigher' in layoutName:
            layoutCls = LayoutGenerators.CubeMapHigherQuality
        elif 'CubeMapMedium' in layoutName:
            layoutCls = LayoutGenerators.CubeMapMediumQuality
        elif 'CubeMap' in layoutName:
            layoutCls = LayoutGenerators.CubeMapLayout
        elif 'PyramidLower' in layoutName:
            layoutCls = LayoutGenerators.PyramidLowerQualityLayout
        elif 'PyramidHigher' in layoutName:
            layoutCls = LayoutGenerators.PyramidHigherQualityLayout
        elif 'Pyramid' in layoutName:
            layoutCls = LayoutGenerators.PyramidLayout
        elif 'RhombicDodecaHigher' in layoutName:
            layoutCls = LayoutGenerators.RhombicDodecaHigherQualityLayout
        elif 'RhombicDodecaLower' in layoutName:
            layoutCls = LayoutGenerators.RhombicDodecaLowerQualityLayout
        elif 'RhombicDodecaMedium' in layoutName:
            layoutCls = LayoutGenerators.RhombicDodecaMediumQualityLayout
        elif 'RhombicDodecaEqual' in layoutName:
            layoutCls = LayoutGenerators.RhombicDodecaEqualQualityLayout
        elif 'RhombicDodeca' in layoutName:
            layoutCls = LayoutGenerators.RhombicDodecaLayout
        elif 'EquirectangularTiledHigher' in layoutName:
            layoutCls = LayoutGenerators.EquirectangularTiledHigherQualityLayout
        elif 'EquirectangularTiledMedium' in layoutName:
            layoutCls = LayoutGenerators.EquirectangularTiledMediumQualityLayout
        elif 'EquirectangularTiledLower' in layoutName:
            layoutCls = LayoutGenerators.EquirectangularTiledLowerQualityLayout
        elif 'EquirectangularTiled' in layoutName:
            layoutCls = LayoutGenerators.EquirectangularTiledLayout

        self.layoutList.append(LayoutGen(layoutName, layoutCls))
