#! /bin/sh
source ../kundo2_aware_xgettext.sh

$EXTRACTRC data/*.rc >> rc.cpp
kundo2_aware_xgettext karbon.pot rc.cpp `find . -name \*.cc -o -name \*.cpp -o -name \*.h |egrep -v "plugins/tools/" |egrep -v "plugins/dockers/"`
rm -f rc.cpp
