#! /bin/sh
source ../kundo2_aware_xgettext.sh

# Note: Don't extract sub-directories: specifically not libs, workpackage and plugins.
$EXTRACTRC *.ui *.kcfg *.rc >> rc.cpp
kundo2_aware_xgettext calligraplan.pot *.cpp about/*.cpp kptaboutdata.h
rm -f rc.cpp
