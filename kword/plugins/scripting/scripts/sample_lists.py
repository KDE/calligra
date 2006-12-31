#!/usr/bin/env python

import KWord

#KWord.insertPage( KWord.pageCount() )
#if KWord.pageCount() < 1: KWord.insertPage(0)
fs = KWord.frameSet(0)
doc = fs.textDocument()

doc.setHtml(
    (
        "<h1><font color=\"blue\">Python Sample: Lists</font></h1>"
        "<p>Unsorted list:</p>."
        "<ul>"
        "<li>First item<ol><li>One</li><li>Two</li><li>Three</li></ol></li>"
        "<li>Second item<ul><li>First child item</li><li>Second child item</li></ul></li>"
        "</ul>"
        "<p>Sorted list:</p>."
        "<ol>"
        "<li>First item<ol><li>One</li><li>Two</li><li>Three</li></ol></li>"
        "<li>Second item<ul><li>First child item</li><li>Second child item</li></ul></li>"
        "</ol>"
    )
)

cursor = doc.rootFrame().lastCursorPosition()
#cursor = doc.lastCursor()
cursor.insertBlock()
cursor.insertHtml("<p><b>TextList:</b></p>")
framesetlist = cursor.insertList()
for i in range( 3 ):
    cursor.insertHtml( "nr=%i" % i )
    cursor.insertBlock()

#l = cursor.insertList()
#for i in range(5):
#    c = l.addItem()
#    print "-----> listitem-cursor=%s" % c
