#! /bin/sh
# Messages.sh files must have one instance of the line with:
# 'potfilename=<potfile>.pot'
# potfilename= must be at the start of the line and without spaces.
# It must refer to one pot file only.
# Release scripts rely on this.
potfilename=KarbonTools.pot

source ../../../kundo2_aware_xgettext.sh

$EXTRACTRC ../../ui/widgets/KarbonPatternOptionsWidget.ui filterEffectTool/FilterEffectEditWidget.ui >> rc.cpp
kundo2_aware_xgettext $potfilename rc.cpp *.cpp *.h CalligraphyTool/*.cpp \
    CalligraphyTool/*.h filterEffectTool/*cpp \
    KarbonPatternOptionsWidget.cpp

rm -f rc.cpp
