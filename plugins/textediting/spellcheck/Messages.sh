#! /bin/sh
source ../../../kundo2_aware_xgettext.sh

$EXTRACTRC --tag=string *.ui >> rc.cpp
kundo2_aware_xgettext calligra_textediting_spellcheck.pot *.cpp
rm -f rc.cpp
