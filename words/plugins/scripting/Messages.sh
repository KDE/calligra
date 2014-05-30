#! /bin/sh
$EXTRACTRC *.ui *.rc >> rc.cpp || exit 11
$EXTRACTATTR --attr=collection,text --attr=collection,comment --attr=script,text --attr=script,comment scripts/*.rc >> rc.cpp || exit 12
$XGETTEXT -kkundo2_i18nc:1c,2 -kkundo2_i18ncp:1c,2,3 *.h *.cpp -o $podir/krossmodulewords.pot
