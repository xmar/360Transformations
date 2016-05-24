
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

    def ToDirName(self):
        return 'NbQec{}Res{}Bitrate{}distStep{}ReuseVideo{}'.format(self.nbQec,
                self.res, self.bitrateGoal, self.distStep, self.reuseVideo)

    def ToComment(self):
        return 'Nb QEC = {}, Distance step = {}, NbTest = {}, use HEVC = {}, FlatFixedResolution = {}, inputVideo={}'.format(self.nbQec, self.distStep, self.nbTest, self.reuseVideo, self.res, self.inputVideo.replace('_','-'))
