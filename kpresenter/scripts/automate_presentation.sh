#!/bin/sh
#
# This script uses KPresenter's DCOP interface to automate a presentation
# Every 10 seconds, it goes to the next sequence or page (just as if
# 'space' was pressed)
#
# Idea from Christopher Molnar <molnarc@mandrakesoft.com>
# Implementation by David Faure <faure@kde.org> using Matthias Ettrich dcop shell
#
# Open the presentation in kpresenter, then launch this script
#
appname=`dcop | grep -m 1 ^kpresenter`
#echo $appname

viewname=`dcop $appname | grep ^View`
#echo $viewname

dcop $appname $viewname screenStart

sleep 5;

while true; do
 sleep 10;
 dcop $appname $viewname screenNext
done
