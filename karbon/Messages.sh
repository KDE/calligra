#! /bin/sh
$EXTRACTRC data/*.rc >> rc.cpp
$XGETTEXT rc.cpp `find . -name \*.cc -o -name \*.cpp -o -name \*.h |egrep -v "plugins/tools/" |egrep -v "plugins/dockers/"` -o $podir/karbon.pot
