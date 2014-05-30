#! /bin/sh
# Note: Don't extract sub-directories: specifically not libs, workpackage and plugins.
$EXTRACTRC *.ui *.kcfg *.rc >> rc.cpp
$XGETTEXT -kkundo2_i18nc:1c,2 -kkundo2_i18ncp:1c,2,3 *.cpp about/*.cpp kptaboutdata.h -o $podir/plan.pot

