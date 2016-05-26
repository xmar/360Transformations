
class Job(object):
    def __init__(self, nbQec, distStep, nbTest, nbFrames, nbDicothomicInteration, reuseVideo, res, inputVideo, bitrateGoal):
        self.nbQec = nbQec
        self.distStep = distStep
        self.nbTest = nbTest
        self.nbFrames = nbFrames
        self.nbDicothomicInteration = nbDicothomicInteration
        self.reuseVideo = reuseVideo
        self.res = res
        self.inputVideo = inputVideo
        self.bitrateGoal = int(bitrateGoal)
        self.printBitrateGoal = self.bitrateGoal != 0

    def ToDirName(self):
        if self.printBitrateGoal:
            return '{}NbQec{}Res{}Bitrate{}distStep{}ReuseVideo{}'.format(self.inputVideo.fileName.split('.')[0],self.nbQec,
                    self.res, self.bitrateGoal, self.distStep, self.reuseVideo)
        else:
            return '{}NbQec{}Res{}distStep{}ReuseVideo{}'.format(self.inputVideo.fileName.split('.')[0],self.nbQec,
                    self.res, self.distStep, self.reuseVideo)


    def ToComment(self):
        return 'Nb QEC = {}, Distance step = {}, NbTest = {}, use HEVC = {}, FlatFixedResolution = {}, BitrateGoal = {} inputVideo={}'.format(self.nbQec, self.distStep, self.nbTest, self.reuseVideo, self.res, self.bitrateGoal, self.inputVideo.fileName.replace('_','-'))
