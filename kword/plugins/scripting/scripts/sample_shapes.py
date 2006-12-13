#!/usr/bin/env python

import KWord

# add KoTextShape frame
textframeset = KWord.addTextFrameSet("mytext")
textframe = textframeset.addTextFrame()
textframe.setBackgroundColor("#ff0000")
textframe.resize(200,100)

doc = textframeset.textDocument()
doc.setHtml("<h1> Text Frame</h1>")

cursor = doc.rootFrame().lastCursorPosition()
cursor.insertBlock()
cursor.insertHtml("<p>AAAAAAAAAAAAAA</p>")

print "shapeId=%s" % textframe.shapeId()
print "isVisible=%s" % textframe.isVisible()
print "width=%s" % textframe.width()
print "height=%s" % textframe.height()
print "positionX=%s" % textframe.positionX()
print "positionY=%s" % textframe.positionY()
print "zIndex=%s" % textframe.zIndex()

#textframe.repaint()
#textframe.setZIndex(3)

# add KoStarShape frame
starframe = KWord.addFrameSet("mystar").addFrame("KoStarShape")
if starframe != None:
    starframe.setPosition(400, 50)
    starframe.rotate(25.0)

# add KoPathShape frame
pathframe = KWord.addFrameSet("mypath").addFrame("KoPathShape")
if pathframe != None:
    pathframe.setPosition(400, 150)
    pathframe.shear(0.5, 0.5)

# add KoRegularPolygonShape frame
polyframe = KWord.addFrameSet("mypoly").addFrame("KoRegularPolygonShape")
if polyframe != None:
    polyframe.setPosition(420, 320)
    polyframe.shear(-0.5, -0.5)

# add TableShape frame
tableframe = KWord.addFrameSet("mytable").addFrame("TableShape")
if tableframe != None:
    tableframe.setPosition(200, 160)
    tableframe.resize(160, 160)
