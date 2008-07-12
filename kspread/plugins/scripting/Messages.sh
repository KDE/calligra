#! /bin/sh
$EXTRACTRC *.ui *.rc >> rc.cpp || exit 11
$XGETTEXT ScriptingWriter.h *.cpp -o $podir/krossmodulekspread.pot
