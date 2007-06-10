#! /bin/sh
$EXTRACTRC --tag-group=koffice extensions/*.xml > xml_doc.cpp
$EXTRACTRC dialogs/*.ui >> rc.cpp
$XGETTEXT *.cpp ui/*.h ui/*.cpp dialogs/*.cc dialogs/*.cpp tests/*.cc tests/*.cpp -o $podir/kspread.pot
rm xml_doc.cpp

