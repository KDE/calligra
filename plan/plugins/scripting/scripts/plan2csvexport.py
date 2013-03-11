#!/usr/bin/env kross
# -*- coding: utf-8 -*-

"""
Python script to export content from a Plan Project to an ods file.

(C)2012 Dag Andersen <danders@get2net.dk>
Licensed under LGPL v2+higher.
"""

import sys, os, traceback
import csv
import Kross, Plan

T = Kross.module("kdetranslation")
Sheets = Kross.module("KSpread")
if Sheets is None:
    raise Exception, T.i18n("Failed to start Sheets. Is Sheets installed?")

class PlanExport:

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
        proj = Plan.project()
        data = self.showDataSelectionDialog( Plan )
        if data is None:
            return
        if len(data) == 0:
            raise Exception, T.i18n("No data to export")

        filename = data[4]
        if not filename:
            raise Exception, T.i18n("You must select a file to write to")

        file = open(filename,'w')
        writer = csv.writer(file)
        self.write2csv(writer, proj, data)
        file.close()

        self.forms.showMessageBox("Information", T.i18n("Information"), T.i18n("Data saved to file:\n%1", [filename]))

    def write2csv(self, writer, proj, data):
        objectType = data[0]
        schedule = data[1]
        props = data[2]
        headers = data[3]
        if len(props) == 0:
            raise Exception, T.i18n("No properties to export")

        if headers == True:
            record = []
            for prop in props:
                record.append(proj.headerData( objectType, prop ))

            if len(record) > 0:
                writer.writerow( record )

        if objectType == 0: # Nodes
            self.exportValues( writer, proj, proj, props, schedule )
        elif objectType == 1: # Resources
            for i in range( proj.resourceGroupCount() ):
                self.exportValues( writer, proj, proj.resourceGroupAt( i ), props, schedule )
        elif objectType == 2: # Accounts
            for i in range( proj.accountCount() ):
                self.exportValues( writer, proj, proj.accountAt( i ), props, schedule )


    def exportValues(self, writer, project, dataobject, props, schedule ):
        record = []
        for prop in props:
            record.append(project.data( dataobject, prop, "DisplayRole", schedule ))

        if len(record) > 0:
            writer.writerow( record )

        for i in range( dataobject.childCount() ):
            self.exportValues( writer, project, dataobject.childAt( i ), props, schedule )

    def showDataSelectionDialog(self, Plan ):
        tabledialog = self.forms.createDialog("Property List")
        tabledialog.setButtons("Ok|Cancel")
        tabledialog.setFaceType("List") #Auto Plain List Tree Tabbed
        
        schedulepage = tabledialog.addPage(T.i18n("Schedules"),T.i18n("Select schedule"))
        schedulewidget = Plan.createScheduleListView(schedulepage)

        sourcepage = tabledialog.addPage(T.i18n("Data"),T.i18n("Select data"))
        sourcewidget = Plan.createDataQueryView(sourcepage)

        savepage = tabledialog.addPage(T.i18n("Save"), T.i18n("Export ods file"),"document-save")
        self.savewidget = self.forms.createFileWidget(savepage, "kfiledialog:///csvexportsave")
        self.savewidget.setMode("Saving")
        self.savewidget.setFilter("*.csv|%(1)s\n*|%(2)s" % { '1' : T.i18n("Spreadsheet"), '2' : T.i18n("All Files") } )

        if tabledialog.exec_loop():
            schedule = schedulewidget.currentSchedule()
            props = sourcewidget.selectedProperties()
            #print "props: ", props
            ot = sourcewidget.objectType()
            #print "objectType: ", ot
            filename = self.savewidget.selectedFile()
            return [ot, schedule, props, sourcewidget.includeHeaders(), filename ]
        return None


PlanExport( self )
