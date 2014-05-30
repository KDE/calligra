#! /bin/sh
$EXTRACTRC *.ui *.rc >> rc.cpp || exit 11
$XGETTEXT -kkundo2_i18nc:1c,2 -kkundo2_i18ncp:1c,2,3 *.cpp -o $podir/sheetssolver.pot
