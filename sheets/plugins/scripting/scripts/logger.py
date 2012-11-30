#!/usr/bin/env kross

import os, time, Kross, KSpread

T = Kross.module("kdetranslation")

class Logger:
    def __init__(self, scriptaction):
        self.scriptaction = scriptaction
        #self.currentpath = self.scriptaction.currentPath()

        #self.undostack = KSpread.undoStack()
        #self.undostack.connect("indexChanged(int)",self.indexChanged)

        #file = os.path(self.getLogDir(),"KSpread.log")

        self.forms = Kross.module("forms")
        self.dialog = self.forms.createDialog(T.i18n("Logger"))
        self.dialog.setButtons("Ok|Cancel")
        self.dialog.setFaceType("Plain") #Auto Plain List Tree Tabbed

        savepage = self.dialog.addPage(T.i18nc("Options page name", "Save"),T.i18n("Save to Log File"),"document-save")
        self.savewidget = self.forms.createFileWidget(savepage, "kfiledialog:///kspreadlogger")
        self.savewidget.setMode("Saving")
        self.savewidget.setFilter("*.txt *.log|%(1)s\n*|%(2)s" % { '1' : T.i18n("Log File"), '2' : T.i18n("All Files") } )

        if self.dialog.exec_loop():
            filename = self.savewidget.selectedFile()
            if os.path.isfile(filename):
                if self.forms.showMessageBox("WarningContinueCancel", T.i18n("Overwrite file?"), T.i18n("The file \"%1\" does already exist. Overwrite the file?", [filename])) != "Continue":
                    raise Exception, T.i18n("Aborted.")
            sheetname = KSpread.currentSheet().sheetName()
            cellrange = "A1:F50" #FIXME
            try:
                self.file = open(filename, "w")
                self.startLogging(sheetname, cellrange)
            except IOError, (errno, strerror):
                raise Exception, T.i18n("Failed to write Log File \"%1\":\n%2", [filename], [strerror])

    def addLog(self, message, flush = True):
        date = time.strftime("%Y-%M-%d %H:%M.%S")
        self.file.write( "%s %s\n" % (date,message) )
        if flush:
            self.file.flush()

    def startLogging(self, sheetname, cellrange = ""):
        self.sheet = KSpread.sheetByName(sheetname)
        self.listener = KSpread.createListener(sheetname, cellrange)
        if not self.listener:
            raise Exception, T.i18n("Failed to create listener for sheetname '%1' and range '%2'", [sheetname], [cellrange])
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
