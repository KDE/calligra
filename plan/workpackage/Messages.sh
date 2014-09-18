#! /bin/sh
source ../../calligra_xgettext.sh

$EXTRACTRC `find . -name \*.ui` `find . -name \*.rc` >> rc.cpp
calligra_xgettext planwork.pot *.cpp aboutdata.h
rm -f rc.cpp
