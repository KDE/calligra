#! /bin/sh
# Messages.sh files must have one instance of the line with:
# 'potfilename=<potfile>.pot'
# potfilename= must be at the start of the line and without spaces.
# It must refer to one pot file only.
# Release scripts rely on this.
potfilename=krossmoduleplan.pot

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
${MSGCAT} -F "$podir/${POT_CPP}" "$podir/${POT_PY}" --use-first > $podir/$potfilename
rm -f "$podir/${POT_CPP}" "$podir/${POT_PY}" rc.cpp
