#! /bin/sh
$EXTRACTRC `find . -name \*.ui` `find . -name \*.rc` >> rc.cpp
$XGETTEXT -kkundo2_i18nc:1c,2 -kkundo2_i18ncp:1c,2,3 *.cpp aboutdata.h -o $podir/planwork.pot

