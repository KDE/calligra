#! /bin/sh
# Messages.sh files must have one instance of the line with:
# 'potfilename=<potfile>.pot'
# potfilename= must be at the start of the line and without spaces.
# It must refer to one pot file only.
# Release scripts rely on this.
potfilename=calligrasheets.pot

source ../kundo2_aware_xgettext.sh

$EXTRACTRC --tag-group=koffice functions/*.xml >> rc.cpp
$EXTRACTRC part/chart/*.ui part/dialogs/*.ui ui/dialogs/*.ui part/*.kcfg part/*.rc >> rc.cpp
$EXTRACTATTR --attr=optionWidget,name ui/CellToolOptionWidgets.xml >> rc.cpp
kundo2_aware_xgettext $potfilename *.cpp engine/*.cpp core/*.cpp core/ksp/*.cpp part/chart/*.cpp part/commands/*.cpp part/dialogs/*.cpp functions/*.cpp part/AboutData.h part/*.cpp part/commands/*.cpp part/dialogs/*.cpp ui/*.cpp ui/commands/*.cpp ui/dialogs/*.cpp ui/strategy/*.cpp
rm -f rc.cpp
