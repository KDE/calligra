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
actionname=`dcop kpresenter qt objects | grep screen_start$ | grep KAction`
dcop kpresenter "$actionname" activate

sleep 5;

actionname=`dcop kpresenter qt objects | grep screen_next$ | grep KAction`
while true; do
 sleep 10;
 dcop kpresenter "$actionname" activate
done
