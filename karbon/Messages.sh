#! /bin/sh
source ../calligra_xgettext.sh

$EXTRACTRC data/*.rc >> rc.cpp
calligra_xgettext karbon.pot rc.cpp `find . -name \*.cc -o -name \*.cpp -o -name \*.h |egrep -v "plugins/tools/" |egrep -v "plugins/dockers/"`
rm -f rc.cpp
