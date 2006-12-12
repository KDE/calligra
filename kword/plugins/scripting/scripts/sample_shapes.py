#!/usr/bin/env python

import KWord

frameset = KWord.addTextFrameSet("myframeset")

######################################################
# add text frame/shape

frame = frameset.addTextFrame()
frame.setBackgroundColor("#ff0000")
frame.resize(200,100)

#fr = frameset.addFrame("TextShapeID")
doc = frameset.textDocument()

doc.setHtml("<h1> Text Frame</h1>")
#fs.addFrame()

cursor = doc.rootFrame().lastCursorPosition()
#cursor = doc.lastCursor()
cursor.insertBlock()
cursor.insertHtml("<p>AAAAAAAAAAAAAA</p>")

print "shapeId=%s" % frame.shapeId()
print "isVisible=%s" % frame.isVisible()
print "width=%s" % frame.width()
print "height=%s" % frame.height()
print "positionX=%s" % frame.positionX()
print "positionY=%s" % frame.positionY()

frame.repaint()

######################################################
# add text frame/shape

starframe = KWord.addFrame("mystar","KoStarShape")
if starframe != None:
    starframe.setPosition(200,50)

pathframe = KWord.addFrame("mypath","KoPathShape")
if pathframe != None:
    pathframe.setPosition(200,200)

polyframe = KWord.addFrame("mypath","KoRegularPolygonShape")
if polyframe != None:
    polyframe.setPosition(200,350)
