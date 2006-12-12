#!/usr/bin/env python

import KWord, time

#KWord.insertPage( KWord.pageCount() )
KWord.insertPage(0)

fs = KWord.frameSet(0)
doc = fs.textDocument()

doc.setHtml(
    (
        "<h1><font color=\"blue\">Python Test Script</font></h1>"
        "<p><i>italic</i> and <b>bold</b> and <u>underlined</u> and a <a href=\"test\">link</a></p>."
        "<ul>"
        "<li>Time=<b>%s</b></li>"
        "<li>pageCount=<b>%s</b></li>"
        "<li>frameSetCount=<b>%s</b></li>"
        "<li>document.url=<b>%s</b></li>"
        "<li>Title=<b>%s</b></li>"
        "<li>Subject=<b>%s</b></li>"
        "<li>Author=<b>%s</b></li>"
        "<li>EMail=<b>%s</b></li>"
        "<li>Keywords=<b>%s</b></li>"
        "</ul>"
        "<pre>Some text in a pre-tag</pre>"
        "<blockquote>Some text in a blockquote-tag</blockquote>"
    ) % (
        time.strftime('%H:%M.%S'),
        KWord.pageCount(),
        KWord.frameSetCount(),
        KWord.document().url(),
        KWord.document().documentInfoTitle(),
        KWord.document().documentInfoSubject(),
        KWord.document().documentInfoAuthorName(),
        KWord.document().documentInfoEmail(),
        KWord.document().documentInfoKeywords(),
    )
)

#cursor = doc.lastCursor()
cursor = doc.rootFrame().lastCursorPosition()

cursor.insertBlock()

table = cursor.insertTable(10,5)
print "========> rows=%s cols=%s" % (table.rows(),table.columns())
for row in range( table.rows() ):
    #table.insertRows(row,1)
    for col in range( table.columns() ):
        c = table.firstCursorPosition(row, col)
        #c.insertBlock()
        c.insertHtml( "(row=%s col=%s)" % (row,col) )

#l = cursor.insertList()
#for i in range(5):
#    c = l.addItem()
#    print "-----> listitem-cursor=%s" % c
