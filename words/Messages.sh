#! /bin/sh
# Collect also the app strings in the part catalog, only a few strings,
# no need for overhead of separate catalog for the app
$EXTRACTRC `find part app  -name \*.ui -o -name \*.rc` >> rc.cpp
$XGETTEXT rc.cpp *.cpp `find part app  -name \*.cpp -o -name \*.cpp -o -name \*.h` -o $podir/words.pot

