#! /bin/sh
$EXTRACTRC `find . -name "*.ui" -o -name "*.rc"` >> rc.cpp || exit 11
$XGETTEXT */*.cpp *.cpp `find . -name "*.h"` -o $podir/kpresenter.pot
