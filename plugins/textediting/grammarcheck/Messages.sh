#! /bin/sh
$EXTRACTRC --tag=string *.ui >> rc.cpp
$XGETTEXT *.cpp -o $podir/GrammarCheckPlugin.pot
