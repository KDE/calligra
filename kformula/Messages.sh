#! /bin/sh
$PREPARETIPS >> rc.cpp
$XGETTEXT rc.cpp *.cc -o $podir/kformula.pot
