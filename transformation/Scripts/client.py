#!/usr/bin/env python3

import argparse
import os
import pickle

import MultiProcess

def main():
    parser = argparse.ArgumentParser(description="Will test mulitple resolution and return the resolution that give a the file size closer to the goad file size");
    parser.add_argument('outputDir', type=str, help='path to the output dir')
    parser.add_argument('--deleteLayout', type=str, help='id to layout to delete', default=None)
    parser.add_argument('--trans',  type=str, help='path to the trans software', default='../build/trans')
    parser.add_argument('--config', type=str, help='path to the generated config file', default='./ConfigTest.ini')
    parser.add_argument('-serverPort', type=int, help='server listen port', default=5042)
    parser.add_argument('hostname', type=str, help='Server hostname')
    parser.add_argument('authkey', type=str, help='Authentification key')
    args = parser.parse_args()

    if args.deleteLayout is None:
        #SpecializedWorker = MultiProcess.FixedBitrateAndFixedDistances( args.trans, args.config, args.outputDir, args.hostname)
        workerArg = MultiProcess.WorkerArg( args.trans, args.config, args.outputDir, args.hostname)

        MultiProcess.RunClient(workerArg, args.hostname, args.serverPort, args.authkey)
    else:
        for (dirpath, dirnames, filenames) in os.walk(args.outputDir):
            for dirname in dirnames:
                if 'QEC' in dirname:
                    path = os.path.join(args.outputDir, dirname, 'quality_storage.dat')
                    print(path)
                    with open(path, 'rb') as i:
                        q = pickle.load(i)
                    for layoutId in q.names:
                        if args.deleteLayout in layoutId:
                            print(layoutId)
                            q.names.remove(layoutId)
                    k = []
                    for layoutId in k:
                        if args.deleteLayout in layoutId:
                            print ('***', layoutId)
                            k.append(layoutId)
                    for layoutId in k:
                        del q.goodQuality[layoutId]

                    k = []
                    for layoutId in k:
                        if args.deleteLayout in layoutId:
                            print ('###', layoutId)
                            k.append(layoutId)
                    for layoutId in k:
                        del q.badQuality[layoutId]
                    with open(path, 'wb') as o:
                        pickle.dump(q,o)

if __name__ == '__main__':
    main()
