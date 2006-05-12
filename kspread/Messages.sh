#! /bin/sh
$EXTRACTRC --tag-group=koffice extensions/*.xml > xml_doc.cc
$EXTRACTRC dialogs/*.ui >> rc.cpp
$XGETTEXT rc.cpp *.cc kspread_aboutdata.h dialogs/*.cc tests/*.cc -o $podir/kspread.pot
rm xml_doc.cc

