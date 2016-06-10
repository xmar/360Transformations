import FoVCenters

class DatasetReader:
    def __init__(self, pathToDatasetFile):
        self.pathToDatasetFile = pathToDatasetFile
        self.headPositionMap = {}
        self.maxTimestamp = 0

    def ReadDataset(self):
        '''Will read the whole dataset file and store the results inside self.headPositionMap'''
        self.headPositionMap = {}
        self.maxTimestamp = 0
        with open(self.pathToDatasetFile,'r') as i:
            for line in i:
                frameId, timestamp, r11, r12, r13, r21, r22, r23, r31, r32, r33 = line.split(',')
                #fovc = FoVCenters.FoVCenters([[r11, r12, r13],[r21, r22, r23],[r31, r32, r33]])
                fovc = FoVCenters.FoVCenters('{},{},{};{},{},{};{},{},{}'.format(r11, r12, r13, r21, r22, r23, r31, r32, r33))
                self.headPositionMap[float(timestamp)] = fovc
                self.maxTimestamp = max(self.maxTimestamp, float(timestamp))

    def GetFoVCenterDict(self):
        return self.headPositionMap

    def ComputeStatistic(self, window):
        '''Compute the max distance traveled during the window time and return the array of max distances'''
        r = []
        keys = sorted(self.headPositionMap.keys())
        keysToLook = sorted(self.headPositionMap.keys())
        for timestamp in sorted(self.headPositionMap.keys()):
            if timestamp <= self.maxTimestamp - window:
                maxD = 0
                for t in keysToLook:
                    if t < timestamp + window:
                        distance = self.headPositionMap[timestamp].OrthodromicDistance(self.headPositionMap[t])
                        maxD = max(distance, maxD)
                keysToLook.remove(timestamp)
                r.append(maxD)
        return r
        
    def ComputeAllPositions(self, window):
        '''Compute all the distances between the FoV center and the initial Fov in the window'''
        r = []
        keys = sorted(self.headPositionMap.keys())
        keysToLook = sorted(self.headPositionMap.keys())
        for timestamp in keys:
            if timestamp <= self.maxTimestamp - window:
                for t in keysToLook:
                    if t > timestamp and t < timestamp + window:
                        distance = self.headPositionMap[timestamp].OrthodromicDistance(self.headPositionMap[t])
                        r.append(distance)
                keysToLook.remove(timestamp)
        return r



