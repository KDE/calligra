#! /bin/sh
$EXTRACTRC *.ui >> rc.cpp
$XGETTEXT -kkundo2_i18nc:1c,2 -kkundo2_i18ncp:1c,2,3 rc.cpp *.cpp -o $podir/CalendarTool.pot
