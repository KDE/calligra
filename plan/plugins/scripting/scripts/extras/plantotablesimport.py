#!/usr/bin/env kross
# -*- coding: utf-8 -*-

"""
Python script to import content from a Plan Project stored
within a Plan file into Tables.

(C)2008 Dag Andersen <danders@get2net.dk>
(C)2011 Dag Andersen <danders@get2net.dk>
Licensed under LGPL v2+higher.
"""

import sys, os, traceback
import Kross, KSpread

T = Kross.module("kdetranslation")

class PlanImport:

    def __init__(self, scriptaction):
        self.scriptaction = scriptaction
        self.currentpath = self.scriptaction.currentPath()
        self.forms = Kross.module("forms")
        try:
            self.start()
        except Exception, inst:
            self.forms.showMessageBox("Sorry", T.i18n("Error"), "%s" % inst)
        except:
            self.forms.showMessageBox("Error", T.i18n("Error"), "%s" % "".join( traceback.format_exception(sys.exc_info()[0],sys.exc_info()[1],sys.exc_info()[2]) ))

    def start(self):
        writer = KSpread.writer()
        filename = self.showImportDialog(writer)
        if not filename:
            return # no exception, user prob pressed cancel

        Plan = Kross.module("Plan")
        if Plan is None:
            raise Exception, T.i18n("Failed to start Plan. Is Plan installed?")

        Plan.openUrl( filename )
        proj = Plan.project()
        data = self.showDataSelectionDialog( writer, Plan )
        if len(data) == 0:
            raise Exception, T.i18n("No data to import")

        objectType = data[0]
        schedule = data[1]
        props = data[2]
        if len(props) == 0:
            raise Exception, T.i18n("No properties to import")

        record = []
        if data[3] == True:
            for prop in props:
                record.append( proj.headerData( objectType, prop ) )
            if not writer.setValues(record):
                if self.forms.showMessageBox("WarningContinueCancel", T.i18n("Warning"), T.i18n("Failed to set all properties of '%1' to cell '%2'", [", ".join(record), writer.cell()])) == "Cancel":
                    return
            writer.next()

        if objectType == 0: # Nodes
            self.importValues( writer, proj, proj, props, schedule )
        if objectType == 1: # Resources
            for i in range( proj.resourceGroupCount() ):
                self.importValues( writer, proj, proj.resourceGroupAt( i ), props, schedule )
        if objectType == 2: # Accounts
            for i in range( proj.accountCount() ):
                self.importValues( writer, proj, proj.accountAt( i ), props, schedule )

    def importValues(self, writer, project, dataobject, props, schedule ):
        record = []
        for prop in props:
            record.append( project.data( dataobject, prop, "DisplayRole", schedule ) )
        if not writer.setValues(record):
            if self.forms.showMessageBox("WarningContinueCancel", T.i18n("Warning"), T.i18n("Failed to set all properties of '%1' to cell '%2'", [", ".join(record), writer.cell()])) == "Cancel":
                return
        writer.next()
        for i in range( dataobject.childCount() ):
            self.importValues( writer, project, dataobject.childAt( i ), props, schedule )

    def showImportDialog(self, writer):
        dialog = self.forms.createDialog(T.i18n("Plan Import"))
        dialog.setButtons("Ok|Cancel")
        dialog.setFaceType("List") #Auto Plain List Tree Tabbed

        openpage = dialog.addPage(T.i18n("Open"),T.i18n("Import from Plan Project File"),"document-import")
        openwidget = self.forms.createFileWidget(openpage, "kfiledialog:///kspreadplanimportopen")
        openwidget.setMode("Opening")
        openwidget.setFilter("*.plan|%(1)s\n*|%(2)s" % { '1' : T.i18n("Plan Project Files"), '2' : T.i18n("All Files") })
        if dialog.exec_loop():
            filename = openwidget.selectedFile()
            if not os.path.isfile(filename):
                raise Exception, T.i18n("No file selected.")
            return filename
        return None

    def showDataSelectionDialog(self, writer, Plan ):
        tabledialog = self.forms.createDialog("Property List")
        tabledialog.setButtons("Ok|Cancel")
        tabledialog.setFaceType("List") #Auto Plain List Tree Tabbed
        
        datapage = tabledialog.addPage(T.i18n("Destination"),T.i18n("Import to sheet beginning at cell"))
        sheetslistview = KSpread.createSheetsListView(datapage)
        sheetslistview.setEditorType("Cell")

        schedulepage = tabledialog.addPage(T.i18n("Schedules"),T.i18n("Select schedule"))
        schedulewidget = Plan.createScheduleListView(schedulepage)

        sourcepage = tabledialog.addPage(T.i18n("Data"),T.i18n("Select data"))
        sourcewidget = Plan.createDataQueryView(sourcepage)
        if tabledialog.exec_loop():
            currentSheet = sheetslistview.sheet()
            if not currentSheet:
                raise Exception, T.i18n("No current sheet.")
            if not writer.setSheet(currentSheet):
                raise Exception, T.i18n("Invalid sheet '%1' defined.", [currentSheet])

            cell = sheetslistview.editor()
            if not writer.setCell(cell):
                raise Exception, T.i18n("Invalid cell '%1' defined.", [cell])

            schedule = schedulewidget.currentSchedule()
            #print "schedule: ", schedule
            props = sourcewidget.selectedProperties()
            #print "props: ", props
            ot = sourcewidget.objectType()
            #print "objectType: ", ot
            return [ot, schedule, props, sourcewidget.includeHeaders() ]
        return None


PlanImport( self )
