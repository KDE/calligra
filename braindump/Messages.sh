#! /bin/sh
$EXTRACTRC `find . -name \*.ui | grep -v '/tests/'` >> rc.cpp
RCFILES=`find . -name \*.rc`
$EXTRACTRC $RCFILES >> rc.cpp

$XGETTEXT rc.cpp `find . -name \*.cc -o -name \*.cpp -o -name \*.h` -o $podir/braindump.pot
