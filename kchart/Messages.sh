#! /bin/sh
# TODO: extract Qt's tr() messages in kdchart/*
$EXTRACTRC *.rc *.kcfg >> rc.cpp
$XGETTEXT *.cpp kchart_aboutdata.h dialogs/*.cpp -o $podir/kchart.pot
