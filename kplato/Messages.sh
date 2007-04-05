#! /bin/sh
$EXTRACTATTR --attr=Label,Text reports/*.ktf >> rc.cpp || exit 11
$XGETTEXT rc.cpp *.cc kptaboutdata.h -o $podir/kplato.pot

