#! /bin/sh
$EXTRACTRC *.ui *.rc >> rc.cpp || exit 11
$XGETTEXT */*.cpp *.cpp *.h KPrAboutData.h -o $podir/kpresenter.pot
