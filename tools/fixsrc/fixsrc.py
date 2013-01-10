#!/usr/bin/python -Qwarnall

#
# This script fixes trivial errors in the sources:
#  - Add a newline to files that don't end in one. (only apply to source files!)
#  - ...more later
#

import sys
import os
import string
import getopt


# Global variables
dryrun = False    # Maybe the default should be True...
recursive = False
verbose = False


def handleFile(name, actions):
    global dryrun, verbose

    infile = open(name)
    contents = infile.read()
    infile.close()

    if verbose:
        sys.stderr.write(name + ":\n")
    if "endswitheol" in actions:
        if contents != "" and contents[-1] != "\n":
            if verbose:
                sys.stderr.write("  Adding EOL to end of file\n")
            contents = contents + "\n"

    if not dryrun:
        outname = name + "--temp"  #FIXME: Generate real tempname
        outfile = open(outname, "w")
        outfile.write(contents)
        outfile.close()
        os.rename(outname, name)


def traverseTree(dir, actions, names):
    global recursive, verbose

    # We could also use os.walk()
    for name in names:
        if dir == "":
            fullname = name
        else:
            fullname = dir + "/" + name
        if not os.path.exists(fullname):
            sys.stderr.write(fullname + ": unknown file or directory\n")
            continue

        if os.path.isdir(fullname):
            if recursive:
                traverseTree(fullname, actions, os.listdir(fullname))
            # Ignore all directories if not in recursive mode
        else:
            handleFile(fullname, actions)


# ================================================================


def usage(errormsg=""):
    if errormsg:
        print "Error:", sys.argv[0] + ":", errormsg, "\n"
    else:
        print "Fix trivial errors in the source tree.\n"

    print """usage: fixsrc [options] [files]
    options:
        -a --actions    a comma separated list of the following possible actions:
                          endswitheol:  adds newline at the end to files that don't end in newline
        -d --dryrun     don't actually perform the actions (combine with --verbose)
                        This is recommended before doing the full run.
        -h --help       print this help and exit immediately
        -r --recursive  recursive: all files that are directories are traversed recursively
        -v --verbose    print extra verbose output

    files:
        source files to be fixed

    example:
        fixsrc.py -rv --actions endswitheol libs
"""
    sys.exit(0)


def main():
    global dryrun, recursive, verbose

    try :
        opts, params = getopt.getopt(sys.argv[1:], "a:dhrv" ,
                                     ["actions", "dryrun", "help", "recursive", "verbose"])
    except getopt.GetoptError:
        usage("unknown options")
    #print "opts", opts
    #print "params", params

    actions = []
    for opt, param in opts:
        if opt in ("-a" , "--actions"):
            actions = string.split(param, ",")
            #print "actions: ", actions
            for a in actions:
                if not a in ["endswitheol"]:
                    usage("unknown action: " + a + "\n\n")
        elif opt in ("-d" , "--dryrun"):
            dryrun = True
        elif opt in ("-h" , "--help"):
            usage()
        elif opt in ("-r" , "--recursive"):
            recursive = True
        elif opt in ("-v" , "--verbose"):
            verbose = True

    if actions == []:
        usage("no actions defined")
    traverseTree("", actions, params)

    return 0

if __name__ == '__main__':
    main()
