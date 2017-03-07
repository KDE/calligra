#! /bin/sh
# Messages.sh files must have one instance of the line with:
# 'potfilename=<potfile>.pot'
# potfilename= must be at the start of the line and without spaces.
# It must refer to one pot file only.
# Release scripts rely on this.
potfilename=karbon.pot

source ../kundo2_aware_xgettext.sh

$EXTRACTRC data/*.rc >> rc.cpp
kundo2_aware_xgettext $potfilename rc.cpp `find . -name \*.cc -o -name \*.cpp -o -name \*.h |egrep -v "plugins/tools/" |egrep -v "plugins/dockers/"`
rm -f rc.cpp
