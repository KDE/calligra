#!/usr/bin/env python

import KWord, time

#KWord.insertPage( KWord.pageCount() )
#if KWord.pageCount() < 1: KWord.insertPage(0)
fs = KWord.frameSet(0)
doc = fs.textDocument()

cursor = doc.rootFrame().lastCursorPosition()
#cursor = doc.lastCursor()

variables = doc.variableNames()

cursor.insertDefaultBlock()
cursor.insertHtml("<b>Variables with Cursor</b><br>")
for n in variables:
    cursor.insertHtml("%s: " % n)
    ok = doc.addVariable(cursor,n)
    cursor.insertDefaultBlock()
    print "VARIABLE ADDED =====> variablename=%s ok=%s" % (n,ok)

for n in variables:
    doc.setVariableValue(n, "%s" % time.strftime('%H:%M.%S'))

cursor.insertDefaultBlock()
cursor.insertHtml("<b>Variables with HTML</b><br>")
cursor.insertHtml("<p>%s</p>" % "".join([ "%s: %s<br>" % (v,doc.variableValue(v)) for v in variables ]))
