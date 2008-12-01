#! /bin/sh
$EXTRACTRC *.ui *.rc >> rc.cpp || exit 11
$EXTRACTATTR --attr=collection,text --attr=collection,comment --attr=script,text --attr=script,comment scripts/*.rc >> rc.cpp || exit 12
$XGETTEXT *.h *.cpp -o $podir/krossmodulekword.pot
