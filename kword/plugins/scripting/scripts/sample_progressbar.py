#!/usr/bin/env python

import KWord, time
komainwindow = KWord.shell()
try:
    for i in range(1,101):
        komainwindow.slotProgress(i)
        time.sleep(0.1)
finally:
    komainwindow.slotProgress(-1)
