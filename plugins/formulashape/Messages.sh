#! /bin/sh
source ../../calligra_xgettext.sh

$EXTRACTRC *.ui >> rc.cpp
calligra_xgettext calligra_shape_formula.pot *.cpp *.h elements/*.cpp elements/*.h
rm -f rc.cpp
