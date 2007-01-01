#!/usr/bin/env python

import KWord

idx = 0
shapes = KWord.shapeKeys()
for shapeId in shapes:
    frame = KWord.addFrame("myshape", shapeId)
    if frame != None:
        #frame.setTextRunAround(tableframe.RunThrough)
        frame.setPosition(idx % 6 * 80 + 20, 100 * idx + 20)
        #frame.resize(160, 160)
        idx += 1
