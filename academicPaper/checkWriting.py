#!/usr/bin/env python3

import argparse
import re

def findWholeWord(w):
    return re.compile(r'\b({0})\b'.format(w), flags=re.IGNORECASE).search

def LatexFilter(line):
    line = line[0:line.find('%')]
    line = re.sub(r'\\ref\{[^\}]+\}','', line)
    line = re.sub(r'\\cite\{[^\\}]+\}','', line)
    line = re.sub(r'\\usetikzlibrary\{[^\\}]+\}','', line)
    line = re.sub(r'\\\w+\b','', line)
    line = re.sub(r'[\{\}]',' ', line)
    return line

def CheckWeasel(fileName):
    """Will check each line of the file named 'fileName' to detected weasel
    words"""
    weaselWords = ['many','various','very','fairly','several','extremely',
            'exceedingly','quite','remarkably','few','surprisingly','mostly',
            'largely','huge', 'tiny','are a number','is a number','excellent',
            'interestingly','significantly','substantially','clearly','vast',
            'relatively','completely', 'will', 'would', 'could', 'may',
            'might','should']
    linecount = 0
    with open(fileName, 'r') as f:
        for line in f:
            linecount += 1
            cline = LatexFilter(line)
            line = line[:-1]
            if any(findWholeWord(word)(cline) is not None for word in weaselWords):
                #then there is a work from the weaselWords list in this line
                for word in weaselWords:
                    if findWholeWord(word)(cline) is not None:
                        pos = line.lower().find(word)
                        if not pos == -1:
                            line = line[:pos] + '\033[1;31m'+ line[pos:pos+len(word)]+'\033[0;0m'+line[pos+len(word):]
                print ('\033[0;33m{}\033[0;36m:\033[0m\033[0;32m{}\033[0;36m:\033[0m {}'.format(fileName,linecount, line)) 

def CheckDoubleWord(fileName):
    """Will check each line of the file named 'fileName' to detected double
    words"""
    linecount = 0
    with open(fileName, 'r') as f:
        for line in f:
            linecount += 1
            cline = LatexFilter(line)
            line = line[:-1]
            lastWord = None
            wordlist = re.findall(r"\b([a-z\-']+)\b", cline.lower())
            for word in wordlist:
                if word == lastWord and len(word) > 1:
                    print ('\033[0;33m{}\033[0;36m:\033[0m\033[0;32m{}\033[0;36m:\033[0m {} -> \033[1;31m{}\033[0;0m'.format(fileName,linecount, line, word))
                lastWord = word

def CheckForPasiveForm(fileName):
    """Will check each line of the file named 'fileName' to detected passive
    forme"""
    linecount = 0
    irregulars=['awoken', 'been','born','beat','become','begun','bent',
    'beset','bet','bid','bidden','bound','bitten','bled','blown','broken',
    'bred','brought','broadcast','built','burnt','burst','bought','cast',
    'caught','chosen','clung','come','cost','crept','cut','dealt','dug',
    'dived','done','drawn','dreamt','driven','drunk','eaten','fallen',
    'fed','felt','fought','found','fit','fled','flung','flown','forbidden',
    'forgotten','foregone','forgiven','forsaken','frozen','gotten','given',
    'gone','ground','grown','hung','heard','hidden','hit','held','hurt',
    'kept','knelt','knit','known','laid','led','leapt','learnt','left',
    'lent','let','lain','lighted','lost','made','meant','met','misspelt',
    'mistaken','mown','overcome','overdone','overtaken','overthrown',
    'paid','pled','proven','put','quit','read','rid','ridden','rung',
    'risen','run','sawn','said','seen','sought','sold','sent','set',
    'sewn','shaken','shaven','shorn','shed','shone','shod','shot','shown',
    'shrunk','shut','sung','sunk','sat','slept','slain','slid','slung',
    'slit','smitten','sown','spoken','sped','spent','spilt','spun','spit',
    'split','spread','sprung','stood','stolen','stuck','stung','stunk',
    'stridden','struck','strung','striven','sworn','swept','swollen',
    'swum','swung','taken','taught','torn','told','thought','thrived',
    'thrown','thrust','trodden','understood','upheld','upset','woken',
    'worn','woven','wed','wept','wound','won','withheld','withstood',
    'wrung','written']
    with open(fileName, 'r') as f:
        for line in f:
            linecount += 1
            cline = LatexFilter(line)
            line = line[:-1]
            lastWord = None
            lineToPrint = line
            somethingToPrint = False
            for word in re.findall(r"\b([a-z0-9\-']+)\b", cline.lower()):
                if lastWord in ['am','are','were','being','is','been','was','be'] and (word[-2:] == 'ed' or word in irregulars):
                    somethingToPrint = True
                    first = True
                    pos = lineToPrint.find('{} {}'.format(lastWord, word))
                    if pos == -1:
                        first = False
                        pos = lineToPrint.find('{}'.format(word))
                    if not pos == -1:
                        l = len(word)+1+len(lastWord) if first else len(lastWord)
                        lineToPrint = lineToPrint[:pos] + '\033[1;31m'+ lineToPrint[pos:pos+l]+'\033[0;0m'+lineToPrint[pos+l:]
                lastWord = word
            if somethingToPrint:
                print ('\033[0;33m{}\033[0;36m:\033[0m\033[0;32m{}\033[0;36m:\033[0m {}'.format(fileName,linecount, lineToPrint))

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Check the writing of a latex file')
    parser.add_argument('files', type=str, nargs='+', help='path to the file to check')
    args = parser.parse_args()

    for fileName in args.files:
        print ('\033[1;0mCheck for weasel words\033[0;0m:')
        CheckWeasel(fileName)
        print ('\033[1;0mCheck for double words\033[0;0m:')
        CheckDoubleWord(fileName)
        print ('\033[1;0mCheck for passives\033[0;0m:')
        CheckForPasiveForm(fileName)

