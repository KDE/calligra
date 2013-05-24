#!/usr/bin/env kross

"""
Python script to import content from a comma-separated-value
file to Sheets.

(C)2007 Sebastian Sauer <mail@dipe.org>
http://kross.dipe.org
http://www.calligra.org/sheets
Dual-licensed under LGPL v2+higher and the BSD license.
"""

import os, datetime, sys, traceback, csv
import Kross, KSpread

T = Kross.module("kdetranslation")

class CsvImport:

    def __init__(self, scriptaction):
        self.scriptaction = scriptaction
        self.currentpath = self.scriptaction.currentPath()

        self.forms = Kross.module("forms")
        self.dialog = self.forms.createDialog(T.i18n("CSV Import"))
        self.dialog.setButtons("Ok|Cancel")
        self.dialog.setFaceType("List") #Auto Plain List Tree Tabbed

        openpage = self.dialog.addPage(T.i18n("Open"),T.i18n("Import from CSV File"),"document-open")
        self.openwidget = self.forms.createFileWidget(openpage, "kfiledialog:///kspreadcsvimportopen")
        self.openwidget.setMode("Opening")
        self.openwidget.setFilter("*.csv *.txt|%(1)s\n*|%(2)s" % { '1' : T.i18n("Comma-Separated-Value Files"), '2' : T.i18n("All Files") } )

        datapage = self.dialog.addPage(T.i18n("Import"),T.i18n("Import to sheet beginning at cell"),"document-import")
        self.sheetslistview = KSpread.createSheetsListView(datapage)
        self.sheetslistview.setEditorType("Cell")

        optionspage = self.dialog.addPage(T.i18n("Options"),T.i18n("Comma Separated Value Options"),"configure")
        self.optionswidget = self.forms.createWidgetFromUIFile(optionspage, os.path.join(self.currentpath, "csvoptions.ui"))

        if self.dialog.exec_loop():
            try:
                self.doImport()
            except:
                self.forms.showMessageBox("Error", T.i18n("Error"), "%s" % "".join( traceback.format_exception(sys.exc_info()[0],sys.exc_info()[1],sys.exc_info()[2]) ))

    def getCustomDialect(self):
        class CustomDialect(csv.excel): pass
        setattr(CustomDialect, 'delimiter', self.optionswidget["DelimiterComboBox"].currentText)
        lineterm = self.optionswidget["LineTerminatorComboBox"].currentText.strip()
        if lineterm == '\\n': setattr(CustomDialect, 'lineterminator', "\n")
        elif lineterm == '\\t': setattr(CustomDialect, 'lineterminator', "\t")
        elif lineterm == '\\r': setattr(CustomDialect, 'lineterminator', "\r")
        elif lineterm == '\\n\\r': setattr(CustomDialect, 'lineterminator', "\n\r")
        elif lineterm == '\\r\\n': setattr(CustomDialect, 'lineterminator', "\r\n")
        else: setattr(CustomDialect, 'lineterminator', lineterm)
        escapechar = self.optionswidget["EscapeCharComboBox"].currentText
        if len(escapechar) != 1: escapechar = None
        setattr(CustomDialect, 'escapechar', escapechar)
        setattr(CustomDialect, 'doublequote', self.optionswidget["DoubleQuoteCheckBox"].checked)
        quotechar = self.optionswidget["QuoteCharComboBox"].currentText
        if len(quotechar) != 1: quotechar = '"'
        setattr(CustomDialect, 'quotechar', quotechar)
        setattr(CustomDialect, 'quoting', self.optionswidget["QuotingCheckBox"].checked)
        setattr(CustomDialect, 'skipinitialspace', self.optionswidget["SkipInitialSpaceCheckBox"].checked)
        setattr(CustomDialect, 'strict', self.optionswidget["StrictCheckBox"].checked)
        return CustomDialect

    def doImport(self):
        currentSheet = self.sheetslistview.sheet()
        if not currentSheet:
            raise Exception, T.i18n("No current sheet.")

        writer = KSpread.writer()
        if not writer.setSheet(currentSheet):
            self.forms.showMessageBox("Sorry", T.i18n("Invalid sheet \"%1\" defined.", [currentSheet]))

        cell = self.sheetslistview.editor()
        if not writer.setCell(cell):
            self.forms.showMessageBox("Sorry", T.i18n("Invalid cell \"%1\" defined.", [cell]))

        csvfilename = self.openwidget.selectedFile()
        if not os.path.isfile(csvfilename):
            self.forms.showMessageBox("Sorry", T.i18n("File '%1' not found.", [csvfilename]))

        #writer.connect("valueChanged()",writer.next)

        csv.register_dialect("custom", self.getCustomDialect())

        csvfile = open(csvfilename,'r')
        try:
            csvreader = csv.reader(csvfile, dialect="custom")
            try:
                while True:
                    record = csvreader.next()
                    if not writer.setValues(record):
                        print "Failed to set all of '%s' to cell '%s'" % (record,writer.cell())
                    #writer.insertValues(record)
                    writer.next()
            except StopIteration:
                pass
        finally:
            csvfile.close()

CsvImport( self )
