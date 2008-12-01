#! /bin/sh
$EXTRACTRC --tag-group=koffice expression/*.xml > xml_doc.cpp
$EXTRACTRC `find mailmerge part -name \*.ui -o -name \*.rc` >> rc.cpp
$XGETTEXT rc.cpp *.cpp `find mailmerge part -name \*.cpp -o -name \*.cpp -o -name \*.h | egrep -v "/old"` -o $podir/kword.pot
rm xml_doc.cpp

