#!/usr/bin/env kross
# -*- coding: utf-8 -*-

"""
Python script to import task data from a spreadsheet

(C)2011 Dag Andersen <danders@get2net.dk>
Licensed under LGPL v2.1+higher.

Requires Calligra Sheets

The data in the spreadsheet must be organized as follows:

1) The data must be found in sheet 1
2) Row 1 must define the header text for each column
3) The header text must match the task data property name
4) Column A must contain the WBS Code
5) Column A *MUST* be set to text format
6) The wbs code must be in the format defined by Tools -> Define WBS Pattern
   (Default is arabic numbers with '.' separator, e.g. 1.2.3)
7) The separator must be a '.' for all levels
8) The tasks must be ordered in increasing wbs code order
9) The WBS codes must be consecutive:
   If you have task 1.2 and 1.4 you must also have tasks 1, 1.1 and 1.3

Example spreadsheet:

WBSCode Name    Responsible Allocation  EstimateType    Estimate    Description
1       A       Elvis                                               Summary task
1.2     B       Elvis       Jane        Effort          3d          Task 
1.1     C       Elvis       John        Duration        4d          Task

2       D       Gardner                                             Summary task
2.1     E       Gardner                                             Summary task
2.1.1   F       Gardner     Jane,John   Effort          3h          Next level
2.1.2   G       Gardner                 Effort          0h          Milestone

(Note that the resources listed in Allocation will be created if they do not already exists)
"""

import os, sys, traceback
import Kross, Plan

T = Kross.module("kdetranslation")

class TaskImporter:

    def __init__(self, scriptaction):
        self.scriptaction = scriptaction

        self.proj = Plan.project()
        
        self.forms = Kross.module("forms")
        self.dialog = self.forms.createDialog(T.i18n("Task Import"))
        self.dialog.setButtons("Ok|Cancel")
        self.dialog.setFaceType("List") #Auto Plain List Tree Tabbed
        
        openpage = self.dialog.addPage(T.i18n("Open"),T.i18n("Tables Tasks"),"document-open")
        self.openwidget = self.forms.createFileWidget(openpage, "kfiledialog:///tablestaskimportopen")
        self.openwidget.setMode("Opening")
        self.openwidget.setFilter("*.ods|%(1)s\n*|%(2)s" % { '1' : T.i18n("Tables"), '2' : T.i18n("All Files") } )

        if self.dialog.exec_loop():
            try:
                Plan.beginCommand( T.i18nc( "(qtundoformat)", "Import tasks" ) )
                self.doImport( self.proj )
                Plan.endCommand()
            except:
                Plan.revertCommand() # in case partly loaded
                self.forms.showMessageBox("Error", T.i18n("Error"), "%s" % "".join( traceback.format_exception(sys.exc_info()[0],sys.exc_info()[1],sys.exc_info()[2]) ))

    def doImport( self, project ):
        filename = self.openwidget.selectedFile()
        if not os.path.isfile(filename):
            raise Exception, T.i18n("No file selected")

        Tables = Kross.module("kspread")
        if not Tables:
            raise Exception, T.i18n("Could not start Calligra Sheets")
        
        if not Tables.openUrl(filename):
            self.forms.showMessageBox("Sorry", T.i18n("Error"), T.i18n("Could not open document: %1", [filename]))
            return

        def findParentTask(parent, wbs):
            if wbs == project.data(parent, 'WBSCode'):
                return parent
            for i in range(parent.childCount()):
                task = findParentTask(parent.childAt(i), wbs)
                if task is not None:
                    return task
            return None
        
        def parentTask(wbs):
            codes = wbs.split('.')
            if len(codes) > 1:
                codes.pop()
                pwbs = '.'.join(codes)
                for i in range(project.taskCount()):
                    parent = findParentTask(project.taskAt(i), pwbs)
                    if parent is not None:
                        return parent
            return project
        
        def createTask(properties, data):
            if len(data) == 0:
                return
            task = project.createTask(parentTask(data[0]))
            if task is None:
                raise Exception, T.i18n("Failed to create task")
            
            for i in range(1, len(properties)):
                project.setData(task, properties[i], data[i])
        
        # Get data into list that can be sorted after wbs code
        reader = Tables.reader()
        props = []
        def changedRow(row):
            if row == 1:
                props.append(reader.currentValues())
            else:
                data = reader.currentValues()
                # skip lines without wbs code
                if data[0] is not None:
                    createTask(props[0], data)

        reader.setSheet("Sheet1")
        reader.connect("changedRow(int)", changedRow)
        reader.start()


TaskImporter( self )
