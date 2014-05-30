#! /bin/sh
$EXTRACTRC ../../ui/widgets/KarbonPatternOptionsWidget.ui filterEffectTool/FilterEffectEditWidget.ui >> rc.cpp
$XGETTEXT -kkundo2_i18nc:1c,2 -kkundo2_i18ncp:1c,2,3 rc.cpp *.cpp *.h CalligraphyTool/*.cpp CalligraphyTool/*.h filterEffectTool/*cpp ../../ui/widgets/KarbonPatternOptionsWidget.cpp \
../../ui/widgets/KarbonGradientEditWidget.cpp -o $podir/KarbonTools.pot
