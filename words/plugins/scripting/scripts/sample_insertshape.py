#!/usr/bin/env kross
# -*- coding: utf-8 -*-

import traceback, Kross, Words

class InsertShape:

    def __init__(self, scriptaction):
        self.scriptaction = scriptaction
        #self.currentpath = self.scriptaction.currentPath()
        self.forms = Kross.module("forms")
        self.dialog = self.forms.createDialog("Insert Shape")
        self.dialog.minimumWidth = 500
        self.dialog.minimumHeight = 360
        self.dialog.setButtons("Ok|Cancel")
        #self.dialog.setFaceType("List") #Auto Plain List Tree Tabbed

        self.shapes = Words.shapeKeys()
        page = self.dialog.addPage("", "")
        widget = self.forms.createWidgetFromUI(page,
            '<ui version="4.0" >'
            ' <class>Form</class>'
            ' <widget class="QWidget" name="Form" >'
            '  <layout class="QHBoxLayout" >'
            '   <item>'
            '    <widget class="QListWidget" name="List">'
            '     <property name="currentRow"><number>%i</number></property>'
            '     %s'
            '    </widget>'
            '   </item>'
            '  </layout>'
            ' </widget>'
            '</ui>'
            % ( 0, ''.join( [ '<item><property name="text" ><string>%s</string></property></item>' % s for s in self.shapes ] ) )
        )
        self.widgetlist = widget["List"]

        if self.dialog.exec_loop():
            self.doInsert()

    def __del__(self):
        self.dialog.delayedDestruct()

    def doInsert(self):
        try:
            shapeId = self.shapes[ self.widgetlist.currentRow ]

            frame = Words.addFrame("myshape", shapeId)
            if frame == None:
                raise "No such shape \"%s\"" % shapeId

            #frame.setTextRunAround(tableframe.RunThrough)
            #frame.setPosition(200, 160)
            #frame.resize(160, 160)

        except:
            message = "".join( traceback.format_exception(sys.exc_info()[0],sys.exc_info()[1],sys.exc_info()[2]) )
            self.forms.showMessageBox("Error", "Error", "%s" % message)

InsertShape(self)
