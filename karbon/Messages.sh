#! /bin/sh
$EXTRACTRC data/*.rc >> rc.cpp
$XGETTEXT rc.cpp *.h *.cpp *.cc */*.cc common/*/*.cpp ui/*/*.cpp plugins/*/*.cpp -o $podir/karbon.pot

