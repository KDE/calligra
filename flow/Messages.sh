#! /bin/sh
$EXTRACTRC `find . -name \*.ui -o -name \*.rc` >> rc.cpp || exit 11
$XGETTEXT -kkundo2_i18nc:1c,2 -kkundo2_i18ncp:1c,2,3 `find . -name \*.cpp -not -name \*.moc.\*` part/FlowAboutData.h -o $podir/flow.pot

