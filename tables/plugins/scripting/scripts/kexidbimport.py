#!/usr/bin/env kross

"""
Python script to import content from a Kexi Project stored
within a KexiDB into KSpread.

(C)2007 Sebastian Sauer <mail@dipe.org>
http://kross.dipe.org
http://www.calligra.org/kspread
Dual-licensed under LGPL v2+higher and the BSD license.
"""

#import os, datetime, sys, traceback, csv
#import Kross, KSpread

import sys, os, traceback
import Kross, KSpread

class KexiImport:

    def __init__(self, scriptaction):
        self.scriptaction = scriptaction
        self.currentpath = self.scriptaction.currentPath()
        self.forms = Kross.module("forms")
        self.start()

    def start(self):
        writer = KSpread.writer()

        connection = self.showImportDialog(writer)
        if not connection:
            return

        try:
            print "databaseNames = %s" % connection.databaseNames()
            print "tableNames = %s" % connection.tableNames()
            print "queryNames = %s" % connection.queryNames()

            queryschema = self.showTableDialog(connection)
            if not queryschema:
                return

            print "queryschema.name() = %s" % queryschema.name()
            print "queryschema.caption() = %s" % queryschema.caption()
            print "queryschema.description() = %s" % queryschema.description()

            cursor = connection.executeQuerySchema(queryschema)
            if not cursor:
                raise "Failed to create cursor."
            if not cursor.moveFirst():
                raise "The cursor has no records to read from."

            while not cursor.eof():
                record = []
                for i in range( cursor.fieldCount() ):
                    record.append( cursor.value(i) )
                if writer.setValues(record):
                    writer.next()
                else:
                    print "Failed to set all of '%s' to cell '%s'" % (record,writer.cell())
                cursor.moveNext()
        finally:
            connection.disconnect()

    def createConnection(self, projectfile):
        kexidb = Kross.module("kexidb")
        if not kexidb:
            raise "Failed to load the KexiDB Scripting module. This script needs Kexi to run."
        connectiondata = kexidb.createConnectionDataByFile(projectfile)
        if not connectiondata:
            raise "Invalid Kexi Project File: %s" % projectfile
        connectiondata.setFileName(projectfile)
        connectiondata.setDatabaseName(projectfile)
        driver = kexidb.driver(connectiondata.driverName())
        if not driver:
            raise "No KexiDB driver for: %s" % connectiondata.driverName()
        connection = driver.createConnection(connectiondata)
        if not connection.connect():
            raise "Failed to connect: %s" % connection.lastError()
        if not connection.useDatabase(connectiondata.databaseName()):
            raise "Failed to open database: %s" % connection.lastError()
        return connection

    def showImportDialog(self, writer):
        dialog = self.forms.createDialog("Kexi Import")
        dialog.setButtons("Ok|Cancel")
        dialog.setFaceType("List") #Auto Plain List Tree Tabbed

        openpage = dialog.addPage("Open","Import from Kexi Project File","document-open")
        openwidget = self.forms.createFileWidget(openpage, "kfiledialog:///kspreadkexidbimportopen")
        openwidget.setMode("Opening")
        openwidget.setFilter("*.kexi *.kexis *kexic|Kexi Project Files\n*|All Files")

        datapage = dialog.addPage("Import","Import to sheet beginning at cell","document-import")
        sheetslistview = KSpread.createSheetsListView(datapage)
        sheetslistview.setEditorType("Cell")

        if dialog.exec_loop():
            projectfile = openwidget.selectedFile()
            if not os.path.isfile(projectfile):
                raise "File '%s' not found." % projectfile

            currentSheet = sheetslistview.sheet()
            if not currentSheet:
                raise "No current sheet."
            if not writer.setSheet(currentSheet):
                raise "Invalid sheet \"%s\" defined." % currentSheet

            cell = sheetslistview.editor()
            if not writer.setCell(cell):
                raise "Invalid cell \"%s\" defined." % cell

            connection = self.createConnection(projectfile)
            return connection
        return None

    def showTableDialog(self, connection):
        tabledialog = self.forms.createDialog("Table or Query")
        tabledialog.setButtons("Ok|Cancel")
        tabledialog.setFaceType("List") #Auto Plain List Tree Tabbed
        sourcepage = tabledialog.addPage("Data","Datasource","table")
        items = []
        for s in connection.tableNames(): items.append("table/%s" % s)
        for s in connection.queryNames(): items.append("query/%s" % s)
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
        #print dir(widget)
        self.sourcelist = sourcewidget["List"]
        if tabledialog.exec_loop():
            current = items[ self.sourcelist.currentRow ]
            if current.startswith("table/"):
                schema = connection.tableSchema(current[6:])
                if not schema:
                    raise "No such tableschema \"%s\"" % current[6:]
                return schema.query()
            elif current.startswith("query/"):
                print "QUERY ==============> %s" % current[6:]
                schema = connection.querySchema(current[6:])
                if not schema:
                    raise "No such queryschema \"%s\"" % current[6:]
                return schema
            else:
                raise "Unknown item %s" % current
        return None

        #csvfilename = self.openwidget.selectedFile()
        #if not os.path.isfile(csvfilename):
            #raise "File '%s' not found." % csvfilename
        ##writer.connect("valueChanged()",writer.next)
        #csv.register_dialect("custom", self.getCustomDialect())
        #csvfile = open(csvfilename,'r')
        #try:
            #csvreader = csv.reader(csvfile, dialect="custom")
            #try:
                #while True:
                    #record = csvreader.next()
                    #if not writer.setValues(record):
                        #print "Failed to set all of '%s' to cell '%s'" % (record,writer.cell())
                    ##writer.insertValues(record)
                    #writer.next()
            #except StopIteration:
                #pass
        #finally:
            #csvfile.close()

KexiImport( self )
