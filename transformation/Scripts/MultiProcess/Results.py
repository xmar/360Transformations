
class Results(object):
    def __init__(self, pathToOutputDir, job):
        self.cdfQuality = ''
        with open('{}/cdfQuality.csv'.format(pathToOutputDir),'r') as i:
            for line in i:
                self.cdfQuality += line
        self.distanceQuality = ''
        with open('{}/distanceQuality.csv'.format(pathToOutputDir), 'r') as i:
            for line in i:
                self.distanceQuality += line
        self.distanceQualityPSNR = ''
        with open('{}/distanceQuality_psnr.csv'.format(pathToOutputDir), 'r') as i:
            for line in i:
                self.distanceQualityPSNR += line
        self.job = job
        self.time = -1

    def SetElapsedTime(self, time):
        self.time = time
