#! /bin/sh
source ../../../../kundo2_aware_xgettext.sh

$EXTRACTRC *.ui *.kcfg >> rc.cpp
kundo2_aware_xgettext calligraplan_scheduler_tj.pot *.cpp
rm -f rc.cpp
