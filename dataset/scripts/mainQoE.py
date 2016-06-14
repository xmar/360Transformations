#!/usr/bin/env python3

import argparse

import Processing

if __name__ ==  '__main__':
    parser = argparse.ArgumentParser(description="Script to compute statistics on dataset files")

    parser.add_argument('datasetFolder', type=str, help='path to the folder containing all datasets' )
    parser.add_argument('outputFolder', type=str, help='path to the output folder' )
    parser.add_argument('pathToDistToQoE', type=str, help='path dist to QoE')
    parser.add_argument('pathToQEC', type=str, help='path to the QEC dir')

    args = parser.parse_args()

    Processing.ComputeExpectedQoE(args.datasetFolder, args.outputFolder, args.pathToDistToQoE, args.pathToQEC)
