#!/usr/bin/env kross

"""
Python script to export content from Sheets to a
Kexi Project stored within a KexiDB.

(C)2007 Sebastian Sauer <mail@dipe.org>
http://kross.dipe.org
http://www.calligra.org/sheets
Dual-licensed under LGPL v2+higher and the BSD license.
"""

#import os, datetime, sys, traceback, csv
#import Kross, KSpread

import sys, os, traceback
import Kross, KSpread

T = Kross.module("kdetranslation")

class KexiExport:

    def __init__(self, scriptaction):
        self.scriptaction = scriptaction
        self.currentpath = self.scriptaction.currentPath()
        self.forms = Kross.module("forms")
        self.start()

    def start(self):
        reader = KSpread.reader()

        connection = self.showExportDialog(reader)
        if not connection:
            return

        try:
            print "databaseNames = %s" % connection.databaseNames()
            print "tableNames = %s" % connection.tableNames()

            tableschema = self.showTableDialog(connection)
            if not tableschema:
                return

            fieldlist = tableschema.fieldlist()

            print "tableschema.name() = %s" % tableschema.name()
            print "tableschema.caption() = %s" % tableschema.caption()
            print "tableschema.description() = %s" % tableschema.description()
            print "fieldlist.names() = %s" % fieldlist.names()

            def changedSheet(sheetname):
                print "changedSheet sheetname=%s" % sheetname

            def changedRow(row):
                values = reader.currentValues()
                #values = [ "%s" % v for v in reader.currentValues() ]

                print "changedRow row=%i values=%s" % (row,values)
                if connection.insertRecord(fieldlist, values):
                    pass
                else:
                    print "=> insert failed: %s" % connection.lastError()

            reader.connect("changedSheet(QString)",changedSheet)
            reader.connect("changedRow(int)",changedRow)
            reader.start()

        finally:
            connection.disconnect()

    def createConnection(self, projectfile):
        kexidb = Kross.module("kexidb")
        if not kexidb:
            raise Exception, T.i18n("Failed to load the KexiDB Scripting module. This script needs Kexi to run.")
        connectiondata = kexidb.createConnectionDataByFile(projectfile)
        if not connectiondata:
            raise Exception, T.i18n("Invalid Kexi Project File: %1", [projectfile])
        connectiondata.setFileName(projectfile)
        connectiondata.setDatabaseName(projectfile)
        driver = kexidb.driver(connectiondata.driverName())
        if not driver:
            raise Exception, T.i18n("No KexiDB driver for: %1", [connectiondata.driverName()])
        connection = driver.createConnection(connectiondata)
        if not connection.connect():
            raise Exception, T.i18n("Failed to connect: %1", [connection.lastError()])
        if not connection.useDatabase(connectiondata.databaseName()):
            raise Exception, T.i18n("Failed to open database: %1", [connection.lastError()])
        return connection

    def showExportDialog(self, reader):
        dialog = self.forms.createDialog(T.i18n("Kexi Export"))
        dialog.setButtons("Ok|Cancel")
        dialog.setFaceType("List") #Auto Plain List Tree Tabbed

        savepage = dialog.addPage(T.i18nc("Options page name", "Save"), T.i18n("Export to Kexi Project File"),"document-save")
        savewidget = self.forms.createFileWidget(savepage, "kfiledialog:///kspreadkexidbexport")
        savewidget.setMode("Saving")
        savewidget.setFilter("*.kexi *.kexis *kexic|%(1)s\n*|%(2)s" % { '1' : T.i18n("Kexi Project Files"), '2' : T.i18n("All Files") } )

        datapage = dialog.addPage(T.i18nc("Options page name", "Export"), T.i18n("Export sheets and ranges"),"document-export")
        sheetslistview = KSpread.createSheetsListView(datapage)
        sheetslistview.setSelectionType("MultiSelect")
        sheetslistview.setEditorType("Range")

        if dialog.exec_loop():
            projectfile = savewidget.selectedFile()
            if not os.path.isfile(projectfile):
                raise Exception, T.i18n("File '%1' not found.", [projectfile])
            reader.setSheets( sheetslistview.sheets() )
            connection = self.createConnection(projectfile)
            return connection
        return None

    def showTableDialog(self, connection):
        tabledialog = self.forms.createDialog(T.i18n("Table or Query"))
        tabledialog.setButtons("Ok|Cancel")
        tabledialog.setFaceType("List") #Auto Plain List Tree Tabbed
        sourcepage = tabledialog.addPage(T.i18n("Data"),T.i18n("Datasource"),"table")
        items = connection.tableNames()
        sourcewidget = self.forms.createWidgetFromUI(sourcepage,
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
            % ( 0 , ''.join([ '<item><property name="text" ><string>%s</string></property></item>' % s for s in items ]) )
        )
        self.sourcelist = sourcewidget["List"]
        if tabledialog.exec_loop():
            current = items[ self.sourcelist.currentRow ]
            return connection.tableSchema(current)
        return None

KexiExport( self )
