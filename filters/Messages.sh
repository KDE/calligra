#! /bin/sh
source ../calligra_xgettext.sh

$EXTRACTRC `find . -name \*.ui` >> rc.cpp || exit 11
calligra_xgettext calligrafilters.pot `find . -name \*.cpp -o -name \*.cc`
