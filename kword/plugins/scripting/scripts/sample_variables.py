#!/usr/bin/env python

import KWord

#KWord.insertPage( KWord.pageCount() )
#if KWord.pageCount() < 1: KWord.insertPage(0)
fs = KWord.frameSet(0)
doc = fs.textDocument()

s = ""
for n in doc.variableNames():
    s += "<li>%s</li>" % n

cursor = doc.rootFrame().lastCursorPosition()
#cursor = doc.lastCursor()
cursor.insertBlock()
cursor.insertHtml("<p>%s</p>" % s)
#doc.setHtml(s)
