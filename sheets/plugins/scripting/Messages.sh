#! /bin/sh
$EXTRACTRC scripts/*.ui *.ui *.rc >> rc.cpp || exit 11
$EXTRACTATTR --attr=collection,text --attr=collection,comment --attr=script,text --attr=script,comment scripts/*.rc >> rc.cpp || exit 12
$XGETTEXT ScriptingWriter.h *.cpp -o $podir/krossmodulesheets.pot
$XGETTEXT --language=Python --join scripts/*.py -o $podir/krossmodulesheets.pot
