#! /bin/sh
source ../../kundo2_aware_xgettext.sh

$EXTRACTRC `find . -name \*.ui` >> rc.cpp || exit 11
kundo2_aware_xgettext calligra_shape_video.pot `find . -name \*.cpp`
rm -f rc.cpp
