#! /bin/sh
$EXTRACTRC data/*.rc >> rc.cpp
$XGETTEXT rc.cpp *.h *.cpp *.cc common/*/*.cpp ui/*/*.cpp plugins/*/*.cpp plugins/*/*/*.cpp ui/*.cpp  ui/*.h -o $podir/karbon.pot

