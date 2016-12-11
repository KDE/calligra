#! /bin/sh
source ../kundo2_aware_xgettext.sh

$EXTRACTRC `find . -name \*.ui -o -name \*.rc` >> rc.cpp || exit 11
kundo2_aware_xgettext flow.pot `find . -name \*.cpp -not -name \*.moc.\*` part/FlowAboutData.h
rm -f rc.cpp
