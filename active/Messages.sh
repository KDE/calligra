#! /usr/bin/env bash
source ../calligra_xgettext.sh

$EXTRACTRC `find . -name \*.rc -o -name \*.ui -o -name \*.kcfg` >> rc.cpp
calligra_xgettext `find . -name \*.qml -o -name \*.cpp -o -name \*.h` > $podir/calligraactive.pot
rm -f rc.cpp
