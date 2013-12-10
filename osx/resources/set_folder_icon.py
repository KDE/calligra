#!/usr/bin/python

from AppKit import NSWorkspace
from AppKit import NSImage
import sys

if len(sys.argv) != 3:
    print 'Usage: ./set_folder_icon.py zzz.icns www/'
    sys.exit()

icon = NSImage.alloc().initWithContentsOfFile_(sys.argv[1])
path = sys.argv[2]

NSWorkspace.sharedWorkspace().setIcon_forFile_options_(icon,
                                                       path,
                                                       0)
