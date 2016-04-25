import pickle

class LayoutStorage:
    def __init__(self, layout, a, nbFrames):
        self.layout = layout
        self.a = a
        self.nbFrames = nbFrames

    def Dump(self, fileName):
        with open(fileName, 'wb') as o:
            pickle.dump(self,o)

    @staticmethod
    def Load(fileName):
        with open(fileName, 'rb') as i:
            return pickle.load(i)

