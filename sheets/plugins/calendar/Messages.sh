#! /bin/sh
source ../../../kundo2_aware_xgettext.sh

$EXTRACTRC *.ui >> rc.cpp
kundo2_aware_xgettext calligrasheets_calendar.pot rc.cpp *.cpp
rm -f rc.cpp
