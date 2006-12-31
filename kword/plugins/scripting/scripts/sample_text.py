#!/usr/bin/env python

import KWord, time

#KWord.insertPage( KWord.pageCount() )
#if KWord.pageCount() < 1: KWord.insertPage(0)
fs = KWord.frameSet(0)
doc = fs.textDocument()

doc.setHtml(
    (
        "<h1><font color=\"blue\">Python Sample Script</font></h1>"
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
        "<p>Some more text in a paragraph...</p>"
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

cursor = doc.rootFrame().lastCursorPosition()
#cursor = doc.lastCursor()
cursor.insertBlock()
cursor.insertHtml("<p><b>Framesets:</b></p>")
framesetlist = cursor.insertList()
for i in range( KWord.frameSetCount() ):
    frameset = KWord.frameSet(i)
    shapeids = [frameset.frame(i).shapeId() for i in range(frameset.frameCount())]
    cursor.insertHtml( "(nr=%i name=%s shapeids=%s)<br/>" % (i,frameset.name(),shapeids) )
    cursor.insertBlock()
