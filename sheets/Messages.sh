#! /bin/sh
# Messages.sh files must have one instance of the line with:
# 'potfilename=<potfile>.pot'
# potfilename= must be at the start of the line and without spaces.
# It must refer to one pot file only.
# Release scripts rely on this.
potfilename=calligrasheets.pot

source ../kundo2_aware_xgettext.sh

$EXTRACTRC --tag-group=koffice functions/*.xml > xml_doc.cpp
$EXTRACTRC chart/*.ui dialogs/*.ui  part/dialogs/*.ui *.kcfg *.rc >> rc.cpp
$EXTRACTATTR --attr=optionWidget,name ui/CellToolOptionWidgets.xml >> rc.cpp
kundo2_aware_xgettext $potfilename *.cpp chart/*.cpp commands/*.cpp database/*.cpp dialogs/*.cpp functions/*.cpp part/AboutData.h part/*.cpp part/commands/*.cpp part/dialogs/*.cpp ui/*.cpp
rm -f xml_doc.cpp rc.cpp
