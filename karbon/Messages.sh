#! /bin/sh
$EXTRACTRC data/*.rc >> rc.cpp
$XGETTEXT rc.cpp *.h *.cpp *.cc common/*/*.cpp ui/*/*.cpp plugins/*/*.cpp plugins/*/*/*.cpp ui/*.cpp -o $podir/karbon.pot

