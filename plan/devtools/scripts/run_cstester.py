#! /usr/bin/python -Qwarnall
#
# -*- coding: latin-1 -*-

import sys, os, subprocess

if __name__ == '__main__':
    if sys.argv[1]:
        fnull = open(os.devnull, 'w')
        filepath = sys.argv[1]
        if not os.path.exists(filepath + ".check"):
            subprocess.call(["cstester", "--create", filepath], stdout = fnull, stderr = fnull)
            sys.exit(0)
        else:
            sys.exit(subprocess.call(["cstester", "--verify", filepath], stdout = fnull, stderr = fnull))
    else:
        print "Wrong use of script: parameters missing:" 
        sys.exit(1)
