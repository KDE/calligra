#!/usr/bin/env python

import KWord

#KWord.insertPage( KWord.pageCount() )
#if KWord.pageCount() < 1: KWord.insertPage(0)
fs = KWord.frameSet(0)
doc = fs.textDocument()

doc.setHtml(
    (
        "<h1><font color=\"blue\">Python Sample: Tables</font></h1>"
        "<table>"
        "<tr><td>row1col1</td><td>row1col2</td></tr>"
        "<tr><td>row2col1</td><td>row2col2</td></tr>"
        "</table>"
        "<table border=\"2\">"
        "<tr><td>row1col1</td><td>row1col2</td></tr>"
        "<tr><td>row2col1</td><td>row2col2</td></tr>"
        "</table>"
    )
)

cursor = doc.rootFrame().lastCursorPosition()
#cursor = doc.lastCursor()
cursor.insertBlock()
cursor.insertHtml("<p><b>TextTable:</b></p>")
table = cursor.insertTable(3,3)
table.firstCursorPosition(0, 0).insertHtml("A")
table.firstCursorPosition(0, 1).insertHtml("B")
table.firstCursorPosition(1, 0).insertHtml("C")
table.firstCursorPosition(1, 1).insertHtml("D")
print "========> rows=%s cols=%s" % (table.rows(),table.columns())
#for row in range(0, table.rows()):
    ##table.insertRows(row,1)
    #for col in range(table.columns() -1,0,-1):
        #table.firstCursorPosition(row,col).insertText( "(row=%s col=%s)" % (row,col) )
        ##c.insertBlock()
        ##f = c.insertFrame()
        ##f.firstCursorPosition().insertText( "(row=%s col=%s)" % (row,col) )
        ##c.insertHtml( "(row=%s col=%s)" % (row,col) )
