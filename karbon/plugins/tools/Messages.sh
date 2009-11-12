#! /bin/sh
$EXTRACTRC ../../ui/widgets/KarbonPatternOptionsWidget.ui filterEffectTool/FilterEffectEditWidget.ui >> rc.cpp
$XGETTEXT rc.cpp *.cpp *.h CalligraphyTool/*.cpp CalligraphyTool/*.h filterEffectTool/*cpp ../../ui/widgets/KarbonPatternOptionsWidget.cpp \
../../ui/widgets/KarbonGradientEditWidget.cpp -o $podir/KarbonTools.pot
