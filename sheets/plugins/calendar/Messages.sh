#! /bin/sh
source ../../../calligra_xgettext.sh

$EXTRACTRC *.ui >> rc.cpp
calligra_xgettext calligrasheets_calendar.pot rc.cpp *.cpp
rm -f rc.cpp
