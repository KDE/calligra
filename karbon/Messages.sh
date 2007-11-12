#! /bin/sh
$EXTRACTRC data/*.rc >> rc.cpp
$XGETTEXT rc.cpp *.h *.cpp *.cc */*.cc widgets/*.cpp plugins/*/*.cc -o $podir/karbon.pot

