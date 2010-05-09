#! /bin/sh
$EXTRACTRC `find . -name "*.ui" -o -name "*.rc"` >> rc.cpp || exit 11
$XGETTEXT `find . -name "*.h" -o -name "*.cpp" -o $podir/kpresenter.pot
