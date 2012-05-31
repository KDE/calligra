#! /bin/sh
$EXTRACTRC *.ui *.rc >> rc.cpp || exit 11
$EXTRACTATTR --attr=collection,text --attr=collection,comment --attr=script,text --attr=script,comment scripts/*.rc >> rc.cpp || exit 11
$XGETTEXT --force-po *.cpp -o $podir/krossmoduleplan.pot
$XGETTEXT --language=Python --join scripts/*.py -o $podir/krossmoduleplan.pot
