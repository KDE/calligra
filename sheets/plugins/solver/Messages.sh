#! /bin/sh
$EXTRACTRC *.ui *.rc >> rc.cpp || exit 11
$XGETTEXT *.cpp -o $podir/sheetssolver.pot
