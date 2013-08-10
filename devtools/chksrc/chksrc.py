#!/usr/bin/python -Qwarnall

#
# This script checks the source tree for errors on a file and directory level:
#  * File level
#     - Checks that the include guard is the same in the #ifndef and #define
#       It does not check if the include guard is correct in relation to the filename.
#  * Directory level:
#     - Checks for duplicate include guards between different files
#     - Checks for different export macros and export files in the same directory
#

import sys
import os
import string
import getopt
import re
import fnmatch


# Global variables
dryrun = False    # Maybe the default should be True...
pattern = "*"
recursive = True
verbose = False

# Stores include guards in .h files.
includeGuards = {}


# ----------------------------------------------------------------
#                         Individual actions


def checkContents(filename, contents):
    global verbose
    global dryrun
    global includeGuards

    # Compile a regex that matches an ifndef statement.
    ifndefPattern = '^(\\#ifndef\\s*)([^\\s]*)(\\s*)'
    ifndefRegex = re.compile(ifndefPattern)

    # Compile a regex that matches an define statement.
    definePattern = '^(\\#define\\s*)([^\\s]*)(\\s*)'
    defineRegex = re.compile(definePattern)

    lineNo = 1
    ifndefFound = False
    defineFound = False
    ifndefLine = -1
    defineLine = -1
    includeGuard = ""
    for line in contents:
        #print "input: ", line

        # Check if this line is an #ifndef line. Only check the first one.
        # if it is, and it's the first one, assume that it's the include guard and store it.
        if not ifndefFound:
            ifndefs = ifndefRegex.findall(line)
            #print "ifndefs: ", ifndefs
            if len(ifndefs) > 0:
                ifndefFound = True
                ifndefLine = lineNo

                ifndef = ifndefs[0]
                # Here we know it's an include statement.  We should only have found one hit.
                #  ifndef[0] = "#ifndef "
                #  ifndef[1] = the symbol the ifndef statement
                #  ifndef[2] = the rest of the line
                includeGuard = ifndef[1]
        
        # Check if this line is a #define line. Only check the first one after the #ifndef is found
        if ifndefFound and not defineFound:
            defines = defineRegex.findall(line)
            #print "defines: ", defines
            if len(defines) > 0:
                defineFound = True
                defineLine = lineNo

                define = defines[0]
                # Here we know it's an include statement.  We should only have found one hit.
                #  define[0] = "#define "
                #  define[1] = the symbol the define statement
                #  define[2] = the rest of the line
                if define[1] == includeGuard and lineNo == ifndefLine + 1:
                    includeGuards[filename] = includeGuard
                else:
                    sys.stderr.write(filename + ":" + str(ifndefLine) + ": Faulty include guard\n")


        # Check for export macros


        lineNo = lineNo + 1

    if not ifndefFound or not defineFound:
        sys.stderr.write(filename + ": No include guard\n")

    return


# ----------------------------------------------------------------


def handleFile(name, actions):
    global dryrun, verbose

    # We only handle .h files for now.
    if name[-2:] != ".h":
        return

    if verbose:
        sys.stderr.write(name + ":\n")

    #print "doing: ", name

    # Read the contents of the file into a list, one item per line.
    infile = open(name)
    contents = infile.readlines()
    infile.close()

    checkContents(name, contents)


def traverseTree(dir, actions, names):
    global recursive, verbose, pattern

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
            if fnmatch.fnmatch(name, pattern):
                handleFile(fullname, actions)


def report():
    global includeGuards

    print
    print "SUMMARY REPORT"
    globalProblems = False

    # Check for duplicate include guards.
    guardFiles = {}
    for file in includeGuards.keys():
        #print file
        guard = includeGuards[file]
        #print "guard:", guard
        if guardFiles.has_key(guard):
            guardFiles[guard].append(file)
        else:
            guardFiles[guard] = [file]
        #print guardFiles

    for guard in guardFiles.keys():
        if len(guardFiles[guard]) > 1:
            globalProblems = True
            sys.stderr.write('include guard "' + guard + '" is duplicated in the following files:\n')
            for file in guardFiles[guard]:
                sys.stderr.write('  ' + file + '\n')

    if not globalProblems:
        print "  No global problems"

    return


# ================================================================


def usage(errormsg=""):
    if errormsg:
        print "Error:", sys.argv[0] + ":", errormsg, "\n"
    else:
        print "Check for errors in the source tree.\n"

    print """usage: chksrc.py [options] [files]
    options:

        -a --actions    a comma separated list of the following possible actions:
                          includeguards: checks the include guards for misdefines and duplicates
                          exports:       normalizes SIGNAL and SLOT signatures
                          all:           all of the above
        -h --help       print this help and exit immediately
        -v --verbse     print verbose output

    files:
        directories to be checked

    examples:
        chksrc.py libs
        chksrc.py .
"""
    sys.exit(0)


def main():
    global verbose
    allActions = ["includeguards", "export", "all"]

    try :
        opts, params = getopt.getopt(sys.argv[1:], "a:hv" ,
                                     ["actions=", "help", "verbose"])
    except getopt.GetoptError:
        usage("unknown options")
    #print "opts:", opts
    #print "params:", params

    actions = []
    for opt, param in opts:
        #print opt, param
        if opt in ("-a" , "--actions"):
            actions = string.split(param, ",")
            #print "actions: ", actions
            for a in actions:
                if not a in allActions:
                    usage("unknown action: " + a + "\n\n")
            if "all" in actions:
                actions = allActions[:-1] # Remove "all", which is a meta action.
                
        elif opt in ("-h" , "--help"):
            usage()
        elif opt in ("-v" , "--verbose"):
            verbose = True

    if actions == []:
        usage("no actions defined")

    # Do the actual work
    traverseTree("", actions, params)
    report()

    return 0

if __name__ == '__main__':
    main()
