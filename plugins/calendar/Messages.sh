#! /bin/sh
$EXTRACTRC *.ui >> rc.cpp
$XGETTEXT rc.cpp *.cpp -o $podir/CalendarTool.pot
