#!/usr/bin/env python

import KWord, time

#KWord.insertPage( KWord.pageCount() )
#if KWord.pageCount() < 1: KWord.insertPage(0)
fs = KWord.frameSet(0)
doc = fs.textDocument()

cursor = doc.rootFrame().lastCursorPosition()
#cursor = doc.lastCursor()

variables = doc.variableNames()

i = 0
while True:
    n = "myVar%i" % i
    if not n in variables:
        ok = doc.addVariable(cursor,n,"%s" % time.strftime('%H:%M.%S'))
        print "VARIABLE ADDED =====> variablename=%s ok=%s" % (n,ok)
        break
    i += 1

cursor.insertDefaultBlock()
cursor.insertHtml("<ul>%s</ul>" % "\n".join([ "<li>%s</li>" % v for v in variables ]))
#doc.setHtml(s)
