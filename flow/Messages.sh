#! /bin/sh
source ../calligra_xgettext.sh

$EXTRACTRC `find . -name \*.ui -o -name \*.rc` >> rc.cpp || exit 11
calligra_xgettext `find . -name \*.cpp -not -name \*.moc.\*` part/FlowAboutData.h > $podir/flow.pot
