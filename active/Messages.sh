#! /usr/bin/env bash
$EXTRACTRC `find . -name \*.rc -o -name \*.ui -o -name \*.kcfg` >> rc.cpp
$XGETTEXT `find . -name "*.qml" -o -name "*.cpp" -o -name "*.h"` -o $podir/calligraactive.pot
rm -f rc.cpp
