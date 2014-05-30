#! /usr/bin/env bash
$EXTRACTRC `find . -name \*.rc -o -name \*.ui -o -name \*.kcfg` >> rc.cpp
$XGETTEXT -kkundo2_i18nc:1c,2 -kkundo2_i18ncp:1c,2,3 `find . -name "*.qml" -o -name "*.cpp" -o -name "*.h"` -o $podir/calligraactive.pot
rm -f rc.cpp
