#! /bin/sh
source ../../kundo2_aware_xgettext.sh

$EXTRACTRC `find . -name \*.ui` >> rc.cpp || exit 11
kundo2_aware_xgettext calligra_shape_music.pot `find . -name \*.cpp -o -name \*.cc`
rm -f rc.cpp
