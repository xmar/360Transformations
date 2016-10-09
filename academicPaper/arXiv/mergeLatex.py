#!/usr/bin/env python3

import argparse
import re

def DetectInputImport(line):
    lineCopy = line[0:line.find('%')] if '%' in line else line
    reInclude = re.compile(r'\s*\\include\{\s*(\S+)\s*\}')
    reInput = re.compile(r'\s*\\input\{\s*(\S+)\s*\}')
    m = reInclude.match(lineCopy)
    if m is not None:
        return m.group(1)
    else:
        m = reInput.match(lineCopy)
        if m is not None:
            return m.group(1)
        else:
            return None

def ReadPlotForPgfName(inputPlot):
    with open(inputPlot) as i:
        lineWithName = re.compile(r'\\tikzsetnextfilename\{\s*(\S+)\s*\}')
        for line in i:
            lineCopy = line[0:line.find('%')] if '%' in line else line
            m = lineWithName.match(lineCopy)
            if m is not None:
                return m.group(1)
    return None

def RecursifRead(outputFile, inputFileName, includeTikzPict, level=1):
    with open(inputFileName, 'r') as i:
        for line in i:
            if '\pgfplotsset{compat=1.8}' in line:
                line = '\pgfplotsset{compat=newest}\n'
            d = DetectInputImport(line)
            if d is None:
                o.write(line)
            else:
                if not includeTikzPict and 'tikzset.tex' in d:
                    continue
                if '/' in inputFileName:
                    nextInput = '{}/{}'.format(inputFileName[0:inputFileName.rfind('/')],d)
                else:
                    nextInput = d
                if includeTikzPict or 'plots/' not in d:
                    print('{}include file {}'.format('\t'*level,d))
                    o.write('%% include file {}\n'.format(d))
                    RecursifRead(outputFile, nextInput, includeTikzPict, level+1)
                    o.write('%% done includind file {}\n'.format(d))
                    print('{}done includind file {}'.format('\t'*level,d))
                else:
                    name = ReadPlotForPgfName(nextInput)
                    pdfFile = name+'.pdf'
                    o.write('\includegraphics{{{}}}\n'.format(pdfFile))
                    print('{}\includegraphics{{{}}}'.format('\t'*level,pdfFile))


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Merge latex file into one file for airXiv");
    parser.add_argument('mainFile', type=str, help='path to the main latex file' )
    parser.add_argument('outputFile', type=str, help='path to output file')
    parser.add_argument('-p', action='store_true', help='also include the tikz pictures. If not set replace them by their pdf name')
    args = parser.parse_args()

    inputFile = args.mainFile
    outputFile = args.outputFile
    includeTikzPict = args.p

    with open(outputFile, 'w') as o:
        print('Start reading {}'.format(inputFile))
        RecursifRead(o, inputFile, includeTikzPict)
