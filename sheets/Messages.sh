#! /bin/sh
source ../calligra_xgettext.sh

$EXTRACTRC --tag-group=koffice functions/*.xml > xml_doc.cpp
$EXTRACTRC chart/*.ui dialogs/*.ui  part/dialogs/*.ui *.kcfg *.rc >> rc.cpp
$EXTRACTATTR --attr=optionWidget,name ui/CellToolOptionWidgets.xml >> rc.cpp
calligra_xgettext calligrasheets.pot *.cpp chart/*.cpp commands/*.cpp database/*.cpp dialogs/*.cpp functions/*.cpp part/AboutData.h part/*.cpp part/commands/*.cpp part/dialogs/*.cpp ui/*.cpp
rm -f xml_doc.cpp rc.cpp
