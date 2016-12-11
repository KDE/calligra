#! /bin/sh
source ../../kundo2_aware_xgettext.sh

$EXTRACTRC *.ui >> rc.cpp
kundo2_aware_xgettext calligra_shape_formula.pot *.cpp *.h elements/*.cpp elements/*.h
rm -f rc.cpp
