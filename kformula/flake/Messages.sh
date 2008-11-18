#! /bin/sh
$EXTRACTRC *.ui  >> rc.cpp
$XGETTEXT *.cpp *.h -o $podir/FormulaShape.pot
