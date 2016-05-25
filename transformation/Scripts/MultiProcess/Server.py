import multiprocessing as mp
from multiprocessing.managers import SyncManager
import queue
import time
import os

import FormatResults
import MultiProcess.Video as Video

def MakeServerManager(port, authkey):
    """ Will create a manager for the server. The server will be located on local host """

    job_q = queue.Queue()
    result_q = queue.Queue()
    server_exit_event = mp.Event()

    class JobQueueManager(SyncManager):
        pass

    JobQueueManager.register('get_job_q', callable=lambda: job_q)
    JobQueueManager.register('get_result_q', callable=lambda: result_q)
    JobQueueManager.register('get_server_exit_event', callable=lambda: server_exit_event)

    manager = JobQueueManager(address=('', port), authkey=authkey.encode('utf-8'))
    manager.start()

    print ('Server started on localhost at port {}'.format(port))
    return manager

def ProcessTheResult(outputDir, result):
    resultOutputDir = '{}/{}'.format(outputDir, result.job.ToDirName())
    if not os.path.exists(resultOutputDir):
        os.mkdir(resultOutputDir)

    with open('{}/cdfQuality.csv'.format(resultOutputDir), 'w') as o:
        o.write(result.cdfQuality)
    with open('{}/distanceQuality.csv'.format(resultOutputDir), 'w') as o:
        o.write(result.distanceQuality)
    with open('{}/distanceQuality_psnr.csv'.format(resultOutputDir), 'w') as o:
        o.write(result.distanceQualityPSNR)
    FormatResults.GeneratePDF(resultOutputDir, '{}/plots'.format(resultOutputDir), result.job.ToComment())

def RunServer(jobList, outputDir, PortAuthkey):
    manager = MakeServerManager(PortAuthkey[0], PortAuthkey[1])
    shared_job_q = manager.get_job_q()
    shared_result_q = manager.get_result_q()
    server_exit_event = manager.get_server_exit_event()

    videoList = []

    listenPort = PortAuthkey[0]+1
    for job in jobList:
        shared_job_q.put(job)
        if not job.inputVideo in videoList:
            videoList.append(job.inputVideo)
    results = []

    videoSender = mp.Process( target= Video.VideoSenderManager, args=(videoList, server_exit_event) )
    videoSender.start()

    try:
        while len(results) != len(jobList):
            result = shared_result_q.get()
            results.append(result)
            ProcessTheResult(outputDir, result)
        server_exit_event.set()
    except KeyboardInterrupt:
        print('Shuting down the server')
        #server_exit_event.set()
        time.sleep(5)
        print('Done')
    finally:
        videoSender.join(10)

    print ('All works done! Ready to stop the server.')

    time.sleep(5)
    manager.shutdown()
