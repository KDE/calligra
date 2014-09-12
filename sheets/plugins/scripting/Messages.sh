#! /bin/sh
source ../../../calligra_xgettext.sh

$EXTRACTRC scripts/*.ui *.ui *.rc >> rc.cpp || exit 11
$EXTRACTATTR --attr=collection,text --attr=collection,comment --attr=script,text --attr=script,comment scripts/*.rc >> rc.cpp || exit 12

old=`pwd`
cd $podir
POT_CPP=$(mktemp cpp_XXXXXXXX.pot)
POT_PY=$(mktemp py_XXXXXXXX.pot)
cd "${old}"
calligra_xgettext "${POT_CPP}" ScriptingWriter.h *.cpp
CXG_EXTRA_ARGS="--language=Python" calligra_xgettext "${POT_PY}" scripts/*.py
${MSGCAT} -F "$podir/${POT_CPP}" "$podir/${POT_PY}" --use-first > $podir/krossmodulesheets.pot
rm -f "$podir/${POT_CPP}" "$podir/${POT_PY}" rc.cpp
