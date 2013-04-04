#!/usr/bin/env python

import subprocess

includes = open("includes2.txt").readlines()
files = open("files_to_check.txt").readlines()

for fn in files:
    fn = fn[:-1]
    print "Checking file", fn
    s = open(fn).read()
    for include in includes:
        include = include[:-1]
        print "Replacing include", include.lower()
        s = s.replace("#include <" + include + ">", "#include <" + include.lower() + ".h>")
    f = open(fn, 'w')
    f.write(s)
    f.flush()