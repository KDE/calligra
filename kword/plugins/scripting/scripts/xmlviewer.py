#!/usr/bin/env kross

import os, sys, re, types, string, datetime, tempfile

import Kross
try:
    import KWord
except ImportError:
    KWord = Kross.module("kword")

    #testcase
    #KWord.document().openUrl("/home/kde4/odf/_works/Lists_bulletedList/testDoc/testDoc.odt")

    if not KWord.document().url():
        forms = Kross.module("forms")
        dialog = forms.createDialog("XML Viewer")
        dialog.setButtons("Ok|Cancel")
        dialog.setFaceType("Plain")
        openwidget = forms.createFileWidget(dialog.addPage("Open","Open ODT File"))
        openwidget.setMode("Opening")
        openwidget.setFilter("*.odt|ODT Files\n*|All Files")
        if not dialog.exec_loop():
            raise "Aborted."
        KWord.document().openUrl(openwidget.selectedFile())

class Dialog:
    def __init__(self, action):
        self.forms = Kross.module("forms")
        self.dialog = self.forms.createDialog("XML Viewer")
        self.dialog.setButtons("Ok")
        self.dialog.setFaceType("List") #Auto Plain List Tree Tabbed
        self.dialog.minimumWidth = 720
        self.dialog.minimumHeight = 500

        doc = KWord.document()
        self.store = KWord.store()
        self.pages = {}

        reader = self.store.open("META-INF/manifest.xml")
        if not reader:
            raise "Failed to read the mainfest"
        for i in range( reader.count() ):
            typeName = reader.type(i)
            path = reader.path(i)
            widgets = []
            if typeName == "text/xml":
                if not self.store.hasFile(path):
                    print "Skipping not existing path %s" % path
                    continue
                page = self.dialog.addPage(path, "")

                browser = self.forms.createWidget(page, "QTextBrowser", "Editor")
                widgets.append(browser)

                w = self.forms.createWidget(page, "QWidget")
                self.forms.createLayout(w,"QHBoxLayout")

                openBtn = self.forms.createWidget(w, "QPushButton")
                openBtn.text = "Open with..."
                openBtn.connect("clicked()", self.openClicked)
                widgets.append(openBtn)

                kwriteBtn = self.forms.createWidget(w, "QPushButton")
                kwriteBtn.text = "Open with KWrite"
                kwriteBtn.connect("clicked(bool)", self.kwriteClicked)
                widgets.append(kwriteBtn)

                kxmleditorBtn = self.forms.createWidget(w, "QPushButton")
                kxmleditorBtn.text = "Open with KXMLEditor"
                kxmleditorBtn.connect("clicked(bool)", self.kxmleditorClicked)
                widgets.append(kxmleditorBtn)

                self.pages[path] = [typeName, None, widgets]

        self.dialog.connect("currentPageChanged(KPageWidgetItem*,KPageWidgetItem*)",self.currentPageChanged)
        self.currentPageChanged()
        self.dialog.exec_loop()

    def __del__(self):
        self.dialog.delayedDestruct()

    def currentPageChanged(self, *args):
        path = self.dialog.currentPage()
        (text,widgets) = self.pages[path][1:3]
        self._text = text
        if self._text:
            return
        self._text = ""
        self._prevLevel = 0
        reader = self.store.open(path)
        if not reader:
            raise "failed to open %s" % path
        def onElement():
            level = reader.level()
            if level > self._prevLevel:
                self._text += "<ul>"
                self._prevLevel = level
            if level < self._prevLevel:
                self._text += "</ul>"
                self._prevLevel = level

            name = reader.name()
            self._text += "<li>%s" % name
            if level > 0:
                attributes = [ "%s=%s" % (n,reader.attribute(n)) for n in reader.attributeNames() ]
                if len(attributes) > 0:
                    self._text += " <small>%s</small>" % ", ".join(attributes)
            #print "  attributeNames=%s" % reader.attributeNames()
            #print "  isElement=%s isText=%s" % (reader.isElement(),reader.isText())
            #if reader.isText():
            #if not reader.hasChildren() and reader.text():
            if reader.name().startswith("text:") and reader.text():
                self._text += "<blockquote>%s</blockquote>" % reader.text()
            self._text += "</li>"
        reader.connect("onElement()", onElement)
        reader.start()
        widgets[0].html = "<style>ul { margin:0; } small { color:#909090; } blockquote { margin:0; color:#000099; }</style>%s" % self._text
        self.pages[path][1] = self._text

    def doOpen(self, program):
        path = self.dialog.currentPage()
        typeName = self.pages[path][0]
        toFile = tempfile.mktemp()
        if typeName == "text/xml":
            toFile += ".xml"

        print "doOpen program=\"%s\" path=\"%s\" typeName=\"%s\" toFile=\"%s\"" % (program,path,typeName,toFile)
        if not self.store.extractToFile(path,toFile):
            raise "Failed to extract \"%s\" to \"%s\"" % (path,toFile)
        if not ( program.startswith('"') and program.endswith('"') ):
            program = "\"%s\"" % program

        result = os.system( "%s \"%s\"" % (program,toFile) )
        if result != 0:
            self.forms.showMessageBox("Error", "Error", "<qt>Failed to execute program:<br><br>%s \"%s\"</qt>" % (program,toFile))

        try:
            if os.path.isfile(toFile):
                os.remove(toFile)
        except:
            pass

    def openClicked(self, *args):
        dialog = self.forms.createDialog("Open with...")
        dialog.setButtons("Ok|Cancel")
        dialog.setFaceType("Plain") #Auto Plain List Tree Tabbed
        dialog.minimumWidth = 360
        page = dialog.addPage("", "")
        edit = self.forms.createWidget(page, "QLineEdit", "Filter")
        edit.text = 'kate'
        edit.setFocus()
        if dialog.exec_loop():
            program = edit.text.strip()
            dialog.delayedDestruct()
            if not program:
                raise "No program defined."
            self.doOpen(program)

    def kwriteClicked(self, *args):
        self.doOpen('kwrite')

    def kxmleditorClicked(self, *args):
        self.doOpen('kxmleditor')

Dialog(self)
