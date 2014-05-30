#! /bin/sh
$EXTRACTRC data/*.rc >> rc.cpp
$XGETTEXT -kkundo2_i18nc:1c,2 -kkundo2_i18ncp:1c,2,3 rc.cpp `find . -name \*.cc -o -name \*.cpp -o -name \*.h |egrep -v "plugins/tools/" |egrep -v "plugins/dockers/"` -o $podir/karbon.pot
