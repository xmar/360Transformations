#!/usr/bin/python3

import argparse
import shutil

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="remove comment lines");
    parser.add_argument('inputFile', type=str, help='path to inputFile' )
    parser.add_argument('tagName', type=str, help='Tag name (for instance XC for \\xc{}' )
    parser.add_argument('-n', action='store_true', help='Remove comments \'%\' except comments started with \'%%\'' )
    args = parser.parse_args()

    tagName = args.tagName
    path = args.inputFile
    removeComments = args.n
    pathTmp = '/tmp/oldVersion.tmp'

    shutil.copy(path, pathTmp)
    shutil.copy(path, '/tmp/'+path.split('/')[-1])

    seen = False
    level = 0
    workToDo = True
    passNb = 0
    while workToDo:
        workToDo = False
        print ('Pass number {}'.format(passNb))
        with open(pathTmp, 'r') as i:
            with open(pathTmp+'.copy', 'w') as o:
                for line in i:
                    if '\{}{{'.format(tagName) in line or removeComments  or seen:
                        workToDo = workToDo or '\{}{{'.format(tagName) in line
                        c = 0
                        while c != len(line):
                            if c+1 != len(line) and line[c] == '%' and (c == 0 or line[c-1] != '\\'):
                                if line [c+1] == '%':
                                    o.write(line[c:])
                                    break
                                else:
                                    #remove line
                                    c = len(line)
                                    break
                            if c+4 <= len(line) and '\{}{{'.format(tagName) == line[c:c+4] and not seen:
                                print(line[c:c+4], end='')
                                c += 4
                                seen = True
                            else:
                                if seen:
                                    if line[c] == '{':
                                        level += 1
                                    elif line[c] == '}' and level > 0:
                                        level -= 1
                                    elif line[c] == '}':
                                        seen = False
                                        if line[c+1:c+3] == '{}':
                                            c += 3
                                        else:
                                            c += 1
                                            print (' No space escape {} detected')
                                        print (' done')
                                        continue
                                o.write(line[c])
                                c += 1
                    else:
                        o.write(line)
        shutil.move(pathTmp+'.copy', pathTmp)
        passNb += 1
    shutil.move(pathTmp, path)
