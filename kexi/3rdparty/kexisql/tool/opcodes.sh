#!/bin/sh

# this script has to be run after the version
# changed

#generate c file:
echo '/* Automatically generated file.  Do not edit */' >../src/opcodes.c
echo 'char *sqliteOpcodeNames[] = { "???", ' >>../src/opcodes.c
grep '^case OP_' ../src/vdbe.c | sed -e 's/^.*OP_/  "/' -e 's/:.*/", /' >>../src/opcodes.c
echo '};' >>../src/opcodes.c

#generate header file:
echo '/* Automatically generated file.  Do not edit */' >../src/opcodes.h
grep '^case OP_' ../src/vdbe.c | sed -e 's/case //' -e 's/:[ {]*//' | awk '{printf "#define %-30s %3d\n", $$2, ++cnt}' >>../src/opcodes.h
