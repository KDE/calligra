#! /bin/sh
$EXTRACTRC --tag-group=koffice functions/*.xml > xml_doc.cpp
$EXTRACTRC chart/*.ui dialogs/*.ui ui/*.ui >> rc.cpp
$XGETTEXT Object.h plugins/scripting/ScriptingWriter.h *.cpp chart/*.cpp commands/*.cpp database/*.cpp dialogs/*.cpp functions/*.cpp part/AboutData.h part/*.cpp ui/*.cpp -o $podir/kspread.pot
rm xml_doc.cpp

