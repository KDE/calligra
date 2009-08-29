#! /bin/sh
$EXTRACTRC `find . -name \*.ui` >> rc.cpp
$XGETTEXT *.cpp kernel/*.cpp kernel/*.h models/*.cpp models/*.h ui/*.cpp ui/*.h -o $podir/kplatolibs.pot
