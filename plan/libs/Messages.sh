#! /bin/sh
source ../../calligra_xgettext.sh

$EXTRACTRC `find . -name \*.ui` >> rc.cpp
calligra_xgettext `find . -name \*.cpp -o -name \*.h` > $podir/planlibs.pot
