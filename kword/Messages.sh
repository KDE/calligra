#! /bin/sh
$EXTRACTRC --tag-group=koffice expression/*.xml > xml_doc.cpp
$EXTRACTRC `find mailmerge -name \*.ui` >> rc.cpp
$XGETTEXT rc.cpp *.cpp `find mailmerge -name \*.cpp -o -name \*.cpp` KWAboutData.h -o $podir/kword.pot
rm xml_doc.cpp

