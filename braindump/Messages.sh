#! /bin/sh
source ../calligra_xgettext.sh

$EXTRACTRC `find . -name \*.ui | grep -v '/tests/'` >> rc.cpp
RCFILES=`find . -name \*.rc`
$EXTRACTRC $RCFILES >> rc.cpp
$EXTRACTATTR --attr=category,name --attr=state,name data/states/states/*.xml >> rc.cpp

calligra_xgettext rc.cpp `find . -name \*.cc -o -name \*.cpp -o -name \*.h` > $podir/braindump.pot
