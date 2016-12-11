#! /bin/sh
source ../../../kundo2_aware_xgettext.sh

$EXTRACTRC *.ui *.rc >> rc.cpp || exit 11
kundo2_aware_xgettext calligrasheets_solver.pot *.cpp
rm -f rc.cpp
