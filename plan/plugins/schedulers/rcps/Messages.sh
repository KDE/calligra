#! /bin/sh
source ../../../../calligra_xgettext.sh

$EXTRACTRC *.ui *.kcfg >> rc.cpp
calligra_xgettext *.cpp > $podir/planrcpsplugin.pot
