#! /bin/sh
$EXTRACTRC `find . -name \*.ui` >> rc.cpp
$XGETTEXT *.cpp aboutdata.h -o $podir/kplatowork.pot

