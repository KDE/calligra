#! /bin/sh
$EXTRACTRC `find . -name "*.ui" -o -name "*.rc"` >> rc.cpp || exit 11
$XGETTEXT `find . -name "*.h" -o -name "*.cpp" | grep -v "/old"` -o $podir/kpresenter.pot
