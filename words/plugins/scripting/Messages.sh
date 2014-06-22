#! /bin/sh
source ../../../calligra_xgettext.sh

$EXTRACTRC *.ui *.rc >> rc.cpp || exit 11
$EXTRACTATTR --attr=collection,text --attr=collection,comment --attr=script,text --attr=script,comment scripts/*.rc >> rc.cpp || exit 12
calligra_xgettext *.h *.cpp > $podir/krossmodulewords.pot
