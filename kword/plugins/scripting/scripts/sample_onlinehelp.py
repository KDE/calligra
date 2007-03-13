#!/usr/bin/env python

import Kross

class Dialog:
    def __init__(self):
        self.forms = Kross.module("forms")
        self.dialog = self.forms.createDialog("KHTML Part")
        self.dialog.minimumWidth = 720
        self.dialog.minimumHeight = 480
        #dialog.setButtons("Ok|Cancel")
        self.dialog.setFaceType("Plain") #Auto Plain List Tree Tabbed
        page = self.dialog.addPage("", "")

        #self.part = self.forms.loadPart(page, "libkhtmlpart", "file:///home/kde4/kspreaddocument2.html")
        #self.part = self.forms.loadPart(page, "libkhtmlpart", "http://127.0.0.1:8282/")
        self.part = self.forms.loadPart(page, "libkhtmlpart", "http://wiki.koffice.org/index.php?title=KWord/Scripting")
        if not self.part:
            raise "Failed to load the KHTML KPart"
        self.part.javaScriptEnabled = False
        self.part.javaEnabled = False
        self.part.pluginsEnabled = False
        #part.openUrl("/home/kde4/kspreaddocument2.html")

        self.part.connect("popupMenu(const QString&, const QPoint&)", self.popupMenu)
        self.part.connect("selectionChanged()", self.selectionChanged)
        self.part.connect("formSubmitNotification(const char*,const QString&,const QByteArray&,const QString&,const QString&, const QString&)", self.formSubmitNotification)

        #print self.part
        #print dir(self.part)
        self.dialog.exec_loop()

    def __del__(self):
        self.dialog.delayedDestruct()

    def popupMenu(self, url, point):
        print "===> popupMenu url=%s point=%s" % (url,point)

    def selectionChanged(self):
        print "===> selectionChanged"

    def formSubmitNotification(self, action, url, formdata, target, contenttype, boundary):
        print "===> formSubmitNotification action=%s url=%s formdata=%s target=%s contenttype=%s boundary=%s" % (action, url, formdata, target, contenttype, boundary)

dialog = Dialog()
dialog.__del__()
