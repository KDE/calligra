#!/usr/bin/env python

import KWord, time

#KWord.insertPage( KWord.pageCount() )
#if KWord.pageCount() < 1: KWord.insertPage(0)
fs = KWord.frameSet(0)
doc = fs.textDocument()

cursor = doc.rootFrame().lastCursorPosition()
#cursor = doc.lastCursor()

variables = doc.variableNames()

for n in variables:
    cursor.insertDefaultBlock()
    cursor.insertHtml("%s: " % n)
    ok = doc.addVariable(cursor,n,"%s" % time.strftime('%H:%M.%S'))
    print "VARIABLE ADDED =====> variablename=%s ok=%s" % (n,ok)

cursor.insertDefaultBlock()
cursor.insertHtml("<p><ul>%s</ul></p>" % "".join([ "<li>%s</li>" % v for v in variables ]))
