#! /bin/sh
# Note: Don't extract sub-directories: specifically not libs, workpackage and plugins.
$EXTRACTRC *.ui *.kcfg >> rc.cpp
$XGETTEXT *.cpp about/*.cpp kptaboutdata.h -o $podir/kplato.pot

