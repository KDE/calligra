#! /bin/sh
source ../kundo2_aware_xgettext.sh

$EXTRACTRC `find . -name "*.ui" -o -name "*.rc"` >> rc.cpp || exit 11
kundo2_aware_xgettext calligrastage.pot `find . -name \*.h -o -name \*.cpp`
rm -f rc.cpp
