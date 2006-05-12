#! /bin/sh
$EXTRACTRC data/*.rc >> rc.cpp
$XGETTEXT rc.cpp *.cpp *.cc */*.cc tools/*.h plugins/*/*.cc -o $podir/karbon.pot

