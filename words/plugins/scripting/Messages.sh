#! /bin/sh
source ../../../kundo2_aware_xgettext.sh

$EXTRACTRC *.ui *.rc >> rc.cpp || exit 11
$EXTRACTATTR --attr=collection,text --attr=collection,comment --attr=script,text --attr=script,comment scripts/*.rc >> rc.cpp || exit 12
kundo2_aware_xgettext krossmodulewords.pot *.h *.cpp
rm -f rc.cpp
