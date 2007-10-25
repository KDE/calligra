#! /bin/sh
$EXTRACTATTR --attr=Label,Text reports/*.ktf >> rc.cpp || exit 11
$XGETTEXT *.cpp kptaboutdata.h -o $podir/kplato.pot

