#! /bin/sh
source ../../kundo2_aware_xgettext.sh

$EXTRACTRC `find . -name \*.ui` `find . -name \*.rc` >> rc.cpp
kundo2_aware_xgettext calligraplanwork.pot *.cpp aboutdata.h
rm -f rc.cpp
