#!/usr/bin/env python3

import argparse

import Processing

if __name__ ==  '__main__':
    parser = argparse.ArgumentParser(description="Script to compute statistics on dataset files")

    parser.add_argument('datasetFolder', type=str, help='path to the folder containing all datasets' )
    parser.add_argument('outputFolder', type=str, help='path to the output folder' )

    args = parser.parse_args()

    Processing.ComputeCdf(args.datasetFolder, args.outputFolder)
