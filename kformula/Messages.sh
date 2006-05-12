#! /bin/sh
$PREPARETIPS >> rc.cpp
$XGETTEXT rc.cpp *.cc kformula_aboutdata.h -o $podir/kformula.pot
