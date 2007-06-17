#!/usr/bin/env kross

"""
Python script to export an OpenDocument Spreadsheet File to
a CSV File.

The script could be used in two ways;

    1. Embedded in KSpread by execution via the "Tools=>Scripts"
       menu or from the "Tools=>Script Manager". In that case
       the document currently loaded and displayed by KSpread
       will be exported to a HTML file.

    2. As standalone script by running;

            # make the script executable
            chmod 755 `kde4-config --install data`/kspread/scripts/extensions/csvexport.py
            # run the script
            `kde4-config --install data`/kspread/scripts/extensions/csvexport.py

       In that case the csvexport.py-script will use the with
       Kross distributed krossrunner commandline-application
       to execute the python script. A empty document will be
       used in that case.

(C)2007 Sebastian Sauer <mail@dipe.org>
http://kross.dipe.org
http://www.koffice.org/kspread
Dual-licensed under LGPL v2+higher and the BSD license.
"""

import os, datetime, sys, traceback, urlparse, csv
import Kross
import myKSpread

class CsvExporter:
    def __init__(self, scriptaction):
        self.scriptaction = scriptaction
        self.currentpath = self.scriptaction.currentPath()

        self.forms = Kross.module("forms")
        self.dialog = self.forms.createDialog("CSV Export")
        self.dialog.setButtons("Ok|Cancel")
        self.dialog.setFaceType("List") #Auto Plain List Tree Tabbed

        savepage = self.dialog.addPage("Save","Export to CSV File","document-save")
        self.savewidget = self.forms.createFileWidget(savepage, "kfiledialog:///kspreadcsvexportsave")
        self.savewidget.setMode("Saving")
        self.savewidget.setFilter("*.csv|CSV Files\n*.txt|Text Files\n*|All Files")

        optionspage = self.dialog.addPage("Options","Comma Separated Value Options","configure")
        self.optionswidget = self.forms.createWidgetFromUIFile(optionspage, os.path.join(self.currentpath, "csvexportoptions.ui"))

        if self.dialog.exec_loop():
            try:
                self.doExport()
            except:
                self.forms.showMessageBox("Error", "Error", "%s" % "".join( traceback.format_exception(sys.exc_info()[0],sys.exc_info()[1],sys.exc_info()[2]) ))

    def doExport(self):
        config = myKSpread.KSpreadReader.Config()
        #readerimpl = myKSpread.KSpreadReader.TestImpl(config)
        readerimpl = myKSpread.KSpreadReader.FileImpl(config)
        reader = myKSpread.KSpreadReader(readerimpl)
        if not reader.hasFile():
            #reader.setFile()
            raise "No Document that could be exported loaded"

        csvfilename = self.savewidget.selectedFile()
        if not csvfilename:
            raise "No CSV file choosen"
        if os.path.splitext(csvfilename)[1] == '':
            csvfilename += '.csv'

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
        csv.register_dialect("custom", CustomDialect)
        dialectname = "custom"

        csvfile = open(csvfilename,'w')
        csvwriter = csv.writer(csvfile, dialect=dialectname)
        try:
            reader.openFile()
            try:
                while True:
                    record = reader.readRecord()
                    if record == None:
                        break
                    if len(record) > 0:
                        print "=> %s" % record
                        csvwriter.writerow( record )
            finally:
                reader.closeFile()
        finally:
            csvfile.close()

CsvExporter( self )
