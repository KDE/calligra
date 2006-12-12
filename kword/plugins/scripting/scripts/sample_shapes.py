#!/usr/bin/env python

import KWord

newframeset = KWord.addTextFrameSet("myframeset")

newframe = newframeset.addTextFrame()
newframe.setBackgroundColor("#ff0000")
newframe.resize(200,200)

#fr = newframeset.addFrame("TextShapeID")
newdoc = newframeset.textDocument()

newdoc.setHtml("<h1>New Text Frame</h1>")
#fs.addFrame()

print "shapeId=%s" % newframe.shapeId()
print "isVisible=%s" % newframe.isVisible()
print "width=%s" % newframe.width()
print "height=%s" % newframe.height()
print "positionX=%s" % newframe.positionX()
print "positionY=%s" % newframe.positionY()

newframe.repaint()
