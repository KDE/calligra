#! /bin/sh
source ../calligra_xgettext.sh

$EXTRACTRC `find . -name "*.ui" -o -name "*.rc"` >> rc.cpp || exit 11
calligra_xgettext calligrastage.pot `find . -name \*.h -o -name \*.cpp`
rm -f rc.cpp
