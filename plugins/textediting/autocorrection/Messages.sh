#! /bin/sh
source ../../../kundo2_aware_xgettext.sh

$EXTRACTRC *.ui >> rc.cpp
kundo2_aware_xgettext calligra_textediting_autocorrect.pot *.cpp
rm -f rc.cpp
