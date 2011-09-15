#!/usr/bin/env kross
# -*- coding: utf-8 -*-

import Words

idx = 0
shapes = Words.shapeKeys()
for shapeId in shapes:
    frame = Words.addFrame("myshape", shapeId)
    if frame != None:
        #frame.setTextRunAround(tableframe.RunThrough)
        frame.setPosition(idx % 6 * 80 + 20, 100 * idx + 20)
        #frame.resize(160, 160)
        idx += 1
