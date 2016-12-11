#! /bin/sh
source ../../../kundo2_aware_xgettext.sh

$EXTRACTRC *.ui *.rc >> rc.cpp || exit 11
$EXTRACTATTR --attr=collection,text --attr=collection,comment --attr=script,text --attr=script,comment scripts/*.rc >> rc.cpp || exit 11

old=`pwd`
cd $podir
POT_CPP=$(mktemp _cpp_XXXXXXXX.pot)
POT_PY=$(mktemp _py_XXXXXXXX.pot)
cd "${old}"
kundo2_aware_xgettext "${POT_CPP}" *.cpp
CXG_EXTRA_ARGS="--language=Python" kundo2_aware_xgettext "${POT_PY}" scripts/*.py
${MSGCAT} -F "$podir/${POT_CPP}" "$podir/${POT_PY}" --use-first > $podir/krossmoduleplan.pot
rm -f "$podir/${POT_CPP}" "$podir/${POT_PY}" rc.cpp
