#! /bin/sh
$EXTRACTRC `find . -name \*.rc` >> rc.cpp
$XGETTEXT `find . -name \*.cpp` -o $podir/krita.pot
