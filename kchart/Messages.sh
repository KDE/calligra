#! /bin/sh
# TODO: extract Qt's tr() messages in kdchart/*
$XGETTEXT *.cpp kchart_aboutdata.h dialogs/*.cpp commands/*.cpp shape/*.cpp -o $podir/kchart.pot
