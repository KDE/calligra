#! /bin/sh
$EXTRACTRC `find . -name \*.ui -o -name \*.rc` >> rc.cpp || exit 11
$XGETTEXT `find . -name \*.cpp -not -name \*.moc.\*` part/FlowAboutData.h -o $podir/flow.pot

