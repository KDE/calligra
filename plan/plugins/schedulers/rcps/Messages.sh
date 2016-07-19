#! /bin/sh
source ../../../../calligra_xgettext.sh

$EXTRACTRC *.ui *.kcfg >> rc.cpp
calligra_xgettext calligraplan_scheduler_rcps.pot *.cpp
rm -f rc.cpp
