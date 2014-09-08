#! /bin/sh
source ../../calligra_xgettext.sh

$EXTRACTRC `find . -name \*.ui` >> rc.cpp
calligra_xgettext planlibs.pot `find . -name \*.cpp -o -name \*.h`
rm -f rc.cpp
