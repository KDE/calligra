#!/usr/bin/env python

import KWord

html = "<ol>"

for i in range( KWord.frameSetCount() ):
    frameset = KWord.frameSet(i)
    html += "<li>name=%s" % frameset.name()
    html += " framecount=%s" % frameset.frameCount()

    doc = frameset.textDocument()
    if doc:
        html += " pagecount=%s modified=%s" % (doc.pageCount(),doc.isModified())

    html2 = ""
    for i in range( frameset.frameCount() ):
        frame = frameset.frame(i)
        html2 += "<li>shapeId=%s" % frame.shapeId()
        html2 += " visible=%s" % frame.isVisible()
        html2 += " scaleX=%s" % frame.scaleX()
        html2 += " scaleY=%s" % frame.scaleY()
        html2 += " rotation=%s" % frame.rotation()
        html2 += " shearX=%s" % frame.shearX()
        html2 += " shearY=%s" % frame.shearY()
        html2 += " width=%s" % frame.width()
        html2 += " height=%s" % frame.height()
        html2 += " positionX=%s" % frame.positionX()
        html2 += " positionY=%s" % frame.positionY()
        html2 += " zIndex=%s" % frame.zIndex()
        html2 += "</li>"

    if html2 != "":
        html += "<ol>%s</ol>" % html2

    html += "</li>"

html += "<ol>"

#KWord.insertPage( KWord.pageCount() )
#if KWord.pageCount() < 1: KWord.insertPage(0)
fs = KWord.frameSet(0)
doc = fs.textDocument()
doc.setHtml(html)
