#! /bin/sh
source ../../../kundo2_aware_xgettext.sh

$EXTRACTRC ../../ui/widgets/KarbonPatternOptionsWidget.ui filterEffectTool/FilterEffectEditWidget.ui >> rc.cpp
kundo2_aware_xgettext KarbonTools.pot rc.cpp *.cpp *.h CalligraphyTool/*.cpp \
    CalligraphyTool/*.h filterEffectTool/*cpp \
    KarbonPatternOptionsWidget.cpp

rm -f rc.cpp
