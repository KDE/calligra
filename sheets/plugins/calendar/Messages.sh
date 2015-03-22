#! /bin/sh
source ../../../calligra_xgettext.sh

$EXTRACTRC *.ui >> rc.cpp
calligra_xgettext sheetscalendartool.pot rc.cpp *.cpp
rm -f rc.cpp
