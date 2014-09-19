#! /bin/sh
source ../../../../calligra_xgettext.sh

$EXTRACTRC *.ui *.kcfg >> rc.cpp
calligra_xgettext planrcpsplugin.pot *.cpp
rm -f rc.cpp
