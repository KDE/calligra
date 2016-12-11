#! /bin/sh
source ../../kundo2_aware_xgettext.sh

$EXTRACTRC *.ui >> rc.cpp
kundo2_aware_xgettext calligra_textinlineobject_variables.pot *.cpp *.ui
rm -f rc.cpp
