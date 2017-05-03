#!/bin/python3


import argparse

def main():
    parser = argparse.ArgumentParser(description="To generate the variables.tex file");
    parser.add_argument('--xavier', action='store_true')
    parser.add_argument('--gwendal', action='store_true')
    parser.add_argument('--alisa', action='store_true')
    parser.add_argument('--fiveG', action='store_true')
    args = parser.parse_args()

    with open('variables.tex', 'w') as o:
        s = '\provideboolean{isXavier}\n'
        s += '\provideboolean{isGwendal}\n'
        s += '\provideboolean{isAlisa}\n'
        s += '\provideboolean{isNone}\n'
        s += '\provideboolean{is5G}\n'

        s += '\setboolean{{isXavier}}{{{}}}\n'.format('true' if args.xavier and not args.gwendal and not args.alisa else 'false')
        s += '\setboolean{{isAlisa}}{{{}}}\n'.format('true' if args.alisa and not args.gwendal and not args.xavier else 'false')
        s += '\setboolean{{isGwendal}}{{{}}}\n'.format('true' if args.gwendal and not args.xavier  and not args.alisa else 'false')
        s += '\setboolean{{is5G}}{{{}}}\n'.format('true' if args.fiveG else 'false')

        o.write(s)

if __name__ == '__main__':
    main()
