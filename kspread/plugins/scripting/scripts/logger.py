#!/usr/bin/env kross

import os, time, Kross, KSpread

class Logger:
    def __init__(self, scriptaction):
        self.scriptaction = scriptaction
        #self.currentpath = self.scriptaction.currentPath()

        #self.undostack = KSpread.undoStack()
        #self.undostack.connect("indexChanged(int)",self.indexChanged)

        #file = os.path(self.getLogDir(),"KSpread.log")

        self.forms = Kross.module("forms")
        self.dialog = self.forms.createDialog("Logger")
        self.dialog.setButtons("Ok|Cancel")
        self.dialog.setFaceType("Plain") #Auto Plain List Tree Tabbed

        savepage = self.dialog.addPage("Save","Save to Log File","document-save")
        self.savewidget = self.forms.createFileWidget(savepage, "kfiledialog:///kspreadlogger")
        self.savewidget.setMode("Saving")
        self.savewidget.setFilter("*.txt *.log|Log File\n*|All Files")

        if self.dialog.exec_loop():
            filename = self.savewidget.selectedFile()
            if os.path.isfile(filename):
                if self.forms.showMessageBox("WarningContinueCancel", "Overwrite file?", "The file \"%s\" does already exist. Overwrite the file?" % filename) != "Continue":
                    raise "Aborted."
            sheetname = KSpread.currentSheet().sheetName()
            cellrange = "A1:F50" #FIXME
            try:
                self.file = open(filename, "w")
                self.startLogging(sheetname, cellrange)
            except IOError, (errno, strerror):
                raise "Failed to write Log File \"%s\":\n%s" % (filename,strerror)

    def addLog(self, message, flush = True):
        date = time.strftime("%Y-%M-%d %H:%M.%S")
        self.file.write( "%s %s\n" % (date,message) )
        if flush:
            self.file.flush()

    def startLogging(self, sheetname, cellrange = ""):
        self.sheet = KSpread.sheetByName(sheetname)
        self.listener = KSpread.createListener(sheetname, cellrange)
        if not self.listener:
            raise "Failed to create listener for sheetname '%s' and range '%s'" % (sheetname,cellrange)
        self.addLog( "Start logging sheet='%s' range='%s'" % (sheetname,cellrange) )
        self.listener.connect("regionChanged(QVariantList)", self.regionChanged)
        self.listener.connect("cellChanged(int,int)", self.cellChanged)

    def regionChanged(self, regions):
        self.lastCount = len(regions)
        print "Logger: Region changed %s" % regions
        self.addLog( "regions=%s" % regions )

    def cellChanged(self, column, row):
        text = self.sheet.text(column, row)
        if self.lastCount > 1:
            flush = False
            self.lastCount -= 1
        else:
            flush = True
        print "Logger: Cell changed column=%i row=%i text=%s" % (column,row,text)
        self.addLog( "column=%i row=%i text=%s" % (column,row,text), flush )

Logger( self )
