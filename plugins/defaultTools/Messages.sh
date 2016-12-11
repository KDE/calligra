#! /bin/sh
source ../../kundo2_aware_xgettext.sh

$EXTRACTRC `find . -name \*.ui` >> rc.cpp
kundo2_aware_xgettext calligra-defaulttools.pot *.cpp */*.cpp
rm -f rc.cpp
