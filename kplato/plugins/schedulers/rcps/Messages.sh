#! /bin/sh
$EXTRACTRC *.ui *.kcfg >> rc.cpp
$XGETTEXT *.cpp -o $podir/kplatorcpsplugin.pot

