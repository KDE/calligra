#! /bin/sh
$EXTRACTRC `find . -name \*.ui | grep -v '/tests/'` >> rc.cpp
RCFILES=`find . -name \*.rc`
$EXTRACTRC $RCFILES >> rc.cpp
$EXTRACTATTR --attr=category,name --attr=state,name data/states/states/*.xml >> rc.cpp

$XGETTEXT -kkundo2_i18nc:1c,2 -kkundo2_i18ncp:1c,2,3 rc.cpp `find . -name \*.cc -o -name \*.cpp -o -name \*.h` -o $podir/braindump.pot
