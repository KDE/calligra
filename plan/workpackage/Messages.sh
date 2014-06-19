#! /bin/sh
source ../../calligra_xgettext.sh

$EXTRACTRC `find . -name \*.ui` `find . -name \*.rc` >> rc.cpp
calligra_xgettext *.cpp aboutdata.h > $podir/planwork.pot
