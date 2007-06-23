#! /bin/sh
$EXTRACTRC --tag-group=koffice extensions/*.xml > xml_doc.cpp
$EXTRACTRC dialogs/*.ui >> rc.cpp
$XGETTEXT Object.h plugins/scripting/ScriptingWriter.h *.cpp chart/*.cpp commands/*.cpp database/*.cpp dialogs/*.cpp functions/*.cpp ui/AboutData.h ui/*.cpp -o $podir/kspread.pot
rm xml_doc.cpp

