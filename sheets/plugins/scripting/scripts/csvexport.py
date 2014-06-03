#!/usr/bin/env kross

"""
Sheets python script to export an ISO OpenDocument spreadsheet file to
a comma-separated-value file.

(C)2007 Sebastian Sauer <mail@dipe.org>
http://kross.dipe.org
http://www.calligra.org/sheets
Dual-licensed under LGPL v2+higher and the BSD license.
"""

import os, datetime, sys, traceback, csv
import Kross, KSpread

T = Kross.module("kdetranslation")

class CsvExporter:

    def __init__(self, scriptaction):
        self.scriptaction = scriptaction
        self.currentpath = self.scriptaction.currentPath()

        self.forms = Kross.module("forms")
        self.dialog = self.forms.createDialog(T.i18n("CSV Export"))
        self.dialog.setButtons("Ok|Cancel")
        self.dialog.setFaceType("List") #Auto Plain List Tree Tabbed

        savepage = self.dialog.addPage(T.i18nc("Options page name", "Save"),T.i18n("Export to CSV File"),"document-save")
        self.savewidget = self.forms.createFileWidget(savepage, "kfiledialog:///kspreadcsvexportsave")
        self.savewidget.setMode("Saving")
        self.savewidget.setFilter("*.csv *.txt|%(1)s\n*|%(2)s" % { '1' : T.i18n("Comma-Separated-Value Files"), '2' : T.i18n("All Files") } )

        datapage = self.dialog.addPage(T.i18nc("Options page name", "Export"),T.i18n("Export Sheets and ranges"),"document-export")
        self.sheetslistview = KSpread.createSheetsListView(datapage)
        self.sheetslistview.setSelectionType("MultiSelect")
        self.sheetslistview.setEditorType("Range")

        optionspage = self.dialog.addPage(T.i18n("Options"),T.i18n("Comma Separated Value Options"),"configure")
        self.optionswidget = self.forms.createWidgetFromUIFile(optionspage, os.path.join(self.currentpath, "csvoptions.ui"))

        if self.dialog.exec_loop():
            try:
                self.doExport()
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

    def doExport(self):
        reader = KSpread.reader()
        reader.setSheets( self.sheetslistview.sheets() )

        #if len(reader.sheetNames()) == 0:
            #raise "No sheet to export selected"

        csvfilename = self.savewidget.selectedFile()
        if not csvfilename:
            raise Exception, T.i18n("No CSV file chosen")
        if os.path.splitext(csvfilename)[1] == '':
            csvfilename += '.csv'

        csv.register_dialect("custom", self.getCustomDialect())

        csvfile = open(csvfilename,'w')
        csvwriter = csv.writer(csvfile, dialect="custom")

        def changedSheet(sheetname):
            print "changedSheet sheetname=%s" % sheetname
            #csvfile.write("# %s\n" % sheetname)

        def changedRow(row):
            values = reader.currentValues()
            #print "changedRow row=%i values=%s" % (row,values)
            csvwriter.writerow(values)

        reader.connect("changedSheet(QString)",changedSheet)
        reader.connect("changedRow(int)",changedRow)
        reader.start()
        csvfile.close()

CsvExporter( self )
