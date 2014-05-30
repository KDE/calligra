#! /bin/sh
# Collect also the app strings in the part catalog, only a few strings,
# no need for overhead of separate catalog for the app
$EXTRACTRC `find part app  -name \*.ui -o -name \*.rc` >> rc.cpp
$XGETTEXT -kkundo2_i18nc:1c,2 -kkundo2_i18ncp:1c,2,3 rc.cpp *.cpp `find part app  -name \*.cpp -o -name \*.cpp -o -name \*.h` -o $podir/words.pot

