import os
import socket
import multiprocessing

class Video(object):
    def __init__(self, path):
        self.realPath = path
        self.fileName = os.path.basename(path)
        self.size = os.path.getsize(path)
        self.port = None

    def SetTcpPort(self, port):
        self.port = port

    def UpdateVideoDir(self, newDir):
        self.realPath = '{}/{}'.format(newDir, self.fileName)

    def Send(self, socket):
        with open(self.realPath, 'rb') as v:
            p = v.read(1024)
            while p:
                socket.send(p)
                p = v.read(1024)

    def Receive(self, socket):
        with open(self.realPath, 'wb') as v:
            p = socket.recv(1024)
            while p:
                v.write(p)
                p = socket.recv(1024)
            print ('Video {} received'.format(self.fileName))


    def __eq__(self, v):
        return v.fileName == self.fileName and self.size == v.size

    def __ne__(self, v):
        return not self.__eq__(v)

def SenderWorker(video, soc):
    try:
        video.Send(soc)
    finally:
        soc.shutdown(socket.SHUT_WR)

def ListenWorker(video, exit_event):
    procs = []
    try:
        s = socket.socket()
        host = '0.0.0.0'
        port = video.port
        print ('Start tcp socket on port {} for video {}'.format(port, video.fileName))
        try:
            s.bind((host, port))
            s.listen(5)
        except:
            s.close()

        while not exit_event.is_set():
            c, addr = s.accept()
            try:
                print('Got connection from {} for the video {}'.format(addr, video.fileName))
                p = multiprocessing.Process( target=SenderWorker, args=(video, c) )
                procs.append(p)
                p.start()
            except KeyboardInterrupt:
                raise
            finally:
                c.close()
    except KeyboardInterrupt:
        raise
    finally:
        for p in procs:
            if p.is_alive():
                p.terminate()
        s.close()

def VideoSenderManager(videoList, exit_event):
    procs = []
    for video in videoList:
        p = multiprocessing.Process( target= ListenWorker, args=(video, exit_event))
        procs.append(p)
        p.start()

    for p in procs:
        p.join()

def VideoReceiver(video, hostname):
    s = socket.socket()
    print('Start connection on {}:{} for video {}'.format(hostname, video.port, video.fileName))
    s.connect((hostname, video.port))
    video.Receive(s)
    s.close()
