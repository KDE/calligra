#! /bin/sh
$EXTRACTRC `find . -name "*.ui" -o -name "*.rc"` >> rc.cpp || exit 11
$XGETTEXT `find . -name "*.h" -o name "*.cpp" | egrep -v "/old"` -o $podir/kpresenter.pot
