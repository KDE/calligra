#! /bin/sh
source ../calligra_xgettext.sh

# Note: Don't extract sub-directories: specifically not libs, workpackage and plugins.
$EXTRACTRC *.ui *.kcfg *.rc >> rc.cpp
calligra_xgettext calligraplan.pot *.cpp about/*.cpp kptaboutdata.h
rm -f rc.cpp
