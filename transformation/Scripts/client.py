#!/usr/bin/env python3

import argparse

import MultiProcess

def main():
    parser = argparse.ArgumentParser(description="Will test mulitple resolution and return the resolution that give a the file size closer to the goad file size");
    parser.add_argument('outputDir', type=str, help='path to the output dir')
    parser.add_argument('--trans',  type=str, help='path to the trans software', default='../build/trans')
    parser.add_argument('--config', type=str, help='path to the generated config file', default='./ConfigTest.ini')
    parser.add_argument('-serverPort', type=int, help='server listen port', default=5042)
    parser.add_argument('hostname', type=str, help='Server hostname')
    parser.add_argument('authkey', type=str, help='Authentification key')
    args = parser.parse_args()

    #SpecializedWorker = MultiProcess.FixedBitrateAndFixedDistances( args.trans, args.config, args.outputDir, args.hostname)
    workerArg = MultiProcess.WorkerArg( args.trans, args.config, args.outputDir, args.hostname)

    MultiProcess.RunClient(workerArg, args.hostname, args.serverPort, args.authkey)

if __name__ == '__main__':
    main()
