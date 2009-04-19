#! /bin/sh
$EXTRACTRC ../../ui/widgets/KarbonPatternOptionsWidget.ui >> rc.cpp
$XGETTEXT rc.cpp *.cpp *.h CalligraphyTool/*.cpp CalligraphyTool/*.h ../../ui/widgets/KarbonPatternOptionsWidget.cpp \
../../ui/widgets/KarbonGradientEditWidget.cpp ../../ui/widgets/KarbonStyleButtonBox.cpp  -o $podir/KarbonTools.pot
