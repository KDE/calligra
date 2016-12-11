#! /bin/sh
source ../../kundo2_aware_xgettext.sh

$EXTRACTRC `find . -name \*.ui` >> rc.cpp
kundo2_aware_xgettext calligraplanlibs.pot `find . -name \*.cpp -o -name \*.h`
rm -f rc.cpp
