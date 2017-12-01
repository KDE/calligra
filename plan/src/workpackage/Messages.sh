#! /bin/sh
# Messages.sh files must have one instance of the line with:
# 'potfilename=<potfile>.pot'
# potfilename= must be at the start of the line and without spaces.
# It must refer to one pot file only.
# Release scripts rely on this.
potfilename=calligraplanwork.pot

source ../../kundo2_aware_xgettext.sh

$EXTRACTRC `find . -name \*.ui` `find . -name \*.rc` >> rc.cpp
kundo2_aware_xgettext $potfilename *.cpp aboutdata.h
rm -f rc.cpp
