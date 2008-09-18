#! /bin/sh
$PREPARETIPS >> rc.cpp
$XGETTEXT rc.cpp `find . -name \*.h -o -name \*.cpp -o -name \*.cc` -o $podir/kformula.pot
