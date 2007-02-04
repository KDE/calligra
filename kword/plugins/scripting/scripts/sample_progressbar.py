#!/usr/bin/env python

import KWord, time
komainwindow = KWord.shell()
komainwindow.slotSetStatusBarText("This is the sample_progressbar.py script")
try:
    for i in range(1,101):
        komainwindow.slotProgress(i)
        time.sleep(0.1)
finally:
    komainwindow.slotSetStatusBarText("")
    komainwindow.slotProgress(-1)
