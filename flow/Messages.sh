#! /bin/sh
source ../calligra_xgettext.sh

$EXTRACTRC `find . -name \*.ui -o -name \*.rc` >> rc.cpp || exit 11
calligra_xgettext flow.pot `find . -name \*.cpp -not -name \*.moc.\*` part/FlowAboutData.h
rm -f rc.cpp
