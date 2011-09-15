#!/usr/bin/env kross
# -*- coding: utf-8 -*-

import Words, time

# fetch the main window
komainwindow = Words.shell()

# set the statusbar text
komainwindow.slotSetStatusBarText("This is the sample_progressbar.py script")

try:
    # increment the progressbar from 0% to 100%
    for i in range(1,101):
        komainwindow.slotProgress(i)
        # wait some seconds else we are to fast ;)
        time.sleep(0.1)
finally:
    # reset the statusbar-text and the progressbar.
    komainwindow.slotSetStatusBarText("")
    komainwindow.slotProgress(-1)
