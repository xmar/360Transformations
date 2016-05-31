class JobArg(object):
    def __init__(self, nbQec, distStep, nbTest, nbFrames, nbDicothomicInteration,
        reuseVideo, res, inputVideo, bitrateGoal):
        self.nbQec = nbQec
        self.distStep = distStep
        self.nbTest = nbTest
        self.nbFrames = nbFrames
        self.nbDicothomicInteration = nbDicothomicInteration
        self.reuseVideo = reuseVideo
        self.res = res
        self.inputVideo = inputVideo
        self.bitrateGoal = int(bitrateGoal)

class GenericJob(object):
    def __init__(self, jobArgs, workerCls):
        ''' jobArb contains the arguments specific to this job (set by the server) '''
        self.jobArgs = jobArgs
        self.workerCls = workerCls

    def SetBitrateGoal(self, bitrateGoal):
        self.jobArgs.bitrateGoal = int(bitrateGoal)

    def RunJob(self, workerArg, shared_job_q, shared_result_q):
        '''workerArg contains the client specific argument for the worker'''
        worker = self.workerCls(workerArg)
        return worker.Run(self, shared_job_q, shared_result_q)

    def ToDirName(self):
        raise NotImplementedError("Class {} doesn't implement ToDirName()".format(
            self.__class__.__name__))

    def ToComment(self):
        raise NotImplementedError("Class {} doesn't implement ToComment()".format(
            self.__class__.__name__))


class Job(GenericJob):
    def __init__(self, jobArgs, workerCls):
        super().__init__(jobArgs, workerCls)
        self.printBitrateGoal = self.jobArgs.bitrateGoal != 0

    def ToDirName(self):
        if self.printBitrateGoal:
            return '{}NbQec{}NbFrames{}Res{}Bitrate{}distStep{}ReuseVideo{}'.format(
                    self.jobArgs.inputVideo.fileName.split('.')[0],
                    self.jobArgs.nbQec, self.jobArgs.nbFrames,
                    self.jobArgs.res, self.jobArgs.bitrateGoal,
                    self.jobArgs.distStep, self.reuseVideo)
        else:
            return '{}NbQec{}NbFrames{}Res{}distStep{}ReuseVideo{}'.format(
                    self.jobArgs.inputVideo.fileName.split('.')[0], self.jobArgs.nbQec,
                    self.jobArgs.nbFrames, self.jobArgs.res, self.jobArgs.distStep,
                    self.jobArgs.reuseVideo)


    def ToComment(self):
        return 'Nb QEC = {}, Nb Frames = {}, Distance step = {}, NbTest = {},\
            use HEVC = {}, FlatFixedResolution = {}, BitrateGoal = {}\
             inputVideo={}'.format(self.jobArgs.nbQec, self.jobArgs.nbFrames,
             self.jobArgs.distStep, self.jobArgs.nbTest, self.jobArgs.reuseVideo,
             self.jobArgs.res, self.jobArgs.bitrateGoal,
             self.jobArgs.inputVideo.fileName.replace('_','-'))
