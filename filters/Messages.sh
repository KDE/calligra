#! /bin/sh
source ../kundo2_aware_xgettext.sh

$EXTRACTRC `find . -name \*.ui` >> rc.cpp || exit 11
kundo2_aware_xgettext calligrafilters.pot `find . -name \*.cpp -o -name \*.cc`
