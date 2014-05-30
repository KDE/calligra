#! /bin/sh
$EXTRACTRC scripts/*.ui *.ui *.rc >> rc.cpp || exit 11
$EXTRACTATTR --attr=collection,text --attr=collection,comment --attr=script,text --attr=script,comment scripts/*.rc >> rc.cpp || exit 12
$XGETTEXT -kkundo2_i18nc:1c,2 -kkundo2_i18ncp:1c,2,3 ScriptingWriter.h *.cpp -o $podir/krossmodulesheets.pot
$XGETTEXT -kkundo2_i18nc:1c,2 -kkundo2_i18ncp:1c,2,3 --language=Python --join scripts/*.py -o $podir/krossmodulesheets.pot
