#! /bin/sh
$EXTRACTRC *.ui *.rc >> ui.cpp || exit 11
$EXTRACTATTR --attr=collection,text --attr=collection,comment --attr=script,text --attr=script,comment scripts/*.rc >> rc.cpp || exit 11
$XGETTEXT *.cpp -o $podir/krossmodulekplato.pot
