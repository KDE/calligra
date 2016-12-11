#! /bin/sh
source ../../kundo2_aware_xgettext.sh

$EXTRACTRC *.ui >> rc.cpp
kundo2_aware_xgettext calligra_shape_artistictext.pot *.cpp *.h
rm -f rc.cpp
