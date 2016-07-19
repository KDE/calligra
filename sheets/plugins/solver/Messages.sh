#! /bin/sh
source ../../../calligra_xgettext.sh

$EXTRACTRC *.ui *.rc >> rc.cpp || exit 11
calligra_xgettext calligrasheets_solver.pot *.cpp
rm -f rc.cpp
