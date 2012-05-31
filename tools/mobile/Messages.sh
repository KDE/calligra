#! /bin/sh
$EXTRACTRC `find . -name \*.ui -o -name \*.rc` >> rc.cpp
$XGETTEXT rc.cpp *.cpp `find . -name \*.cpp -o -name \*.cpp -o -name \*.h` -o $podir/FreOffice.pot

