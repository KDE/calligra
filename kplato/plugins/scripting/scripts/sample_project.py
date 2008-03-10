#!/usr/bin/env kross
# -*- coding: utf-8 -*-

import os, sys, traceback, tempfile, zipfile
import Kross

try:
    # try to import the KPlato application. If this fails we are not running embedded.
    KPlato = __import__("KPlato")
except ImportError:
    # looks as we are not running embedded within the KOffice application. So, let's use Kross to import the library.
    KPlato = Kross.module("KPlato")

    if not KPlato.document().url():
        # if the app does not have a loaded document now we show a fileopen-dialog to let the user choose the file.
        forms = Kross.module("forms")
        dialog = forms.createDialog("Project Viewer")
        dialog.setButtons("Ok|Cancel")
        dialog.setFaceType("Plain")
        openwidget = forms.createFileWidget(dialog.addPage("Open","Open Project File"))
        openwidget.setMode("Opening")
        openwidget.setFilter("*.%s|OpenDocument Files\n*|All Files" % "kplato")
        if not dialog.exec_loop():
            raise Exception("Aborted.")
        KPlato.document().openUrl(openwidget.selectedFile())

proj = KPlato.project()

print
print "Project:", proj.nodeData( proj.projectIdentity(), 'NodeName' )

props = ['NodeName', 'NodeType', 'NodeResponsible' ]
# print the localized headers
for prop in props:
    print "%-20s" % (proj.nodeHeaderData( prop ) ),
print
# Get the identities of all nodes (arbitrary order)
nodes = proj.nodesIdentityList()
for node in nodes:
    # Get the (non-localized) node type
    nodeType = proj.nodeData( node, "NodeType", "EditRole" )
    if nodeType in [ 'Task', 'Milestone' ]:
        for prop in props:
            print "%-20s" % ( proj.nodeData( node, prop ) ),
        print
print
