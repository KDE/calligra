#! /bin/sh
$EXTRACTRC `find . -name \*.ui` `find . -name \*.rc` >> rc.cpp
$XGETTEXT *.cpp aboutdata.h -o $podir/planwork.pot

