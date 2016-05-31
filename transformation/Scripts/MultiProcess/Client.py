import multiprocessing as mp
from multiprocessing.managers import SyncManager
import queue
from .Worker import WorkerManager
from .Video import VideoReceiver


def MakeClientManager(host, port, authkey):
    """Create a manager for the client. This manager will connect to a server and get the shared queues"""

    class ServerQueueManager(SyncManager):
        pass

    ServerQueueManager.register('get_job_q')
    ServerQueueManager.register('get_result_q')
    ServerQueueManager.register('get_server_exit_event')

    manager = ServerQueueManager(address=(host, port), authkey=authkey.encode('utf-8'))
    manager.connect()

    print ('Client connected to {}:{}'.format(host, port))
    return manager


def RunClient(workerArg, host, port, authkey):
    """The function that connect to the server and start to process the jobs. OutputDir is the path to the local output directory. mp_job_worker the function that take the input_queue and the results_queue and process the results."""
    manager = MakeClientManager(host, port, authkey)

    job_q = manager.get_job_q()
    result_q = manager.get_result_q()
    server_exit_event = manager.get_server_exit_event()

    #Do the job (only one process)
    WorkerManager(workerArg, job_q, result_q, server_exit_event)
    del manager
