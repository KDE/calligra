#!/usr/bin/env kross
# -*- coding: utf-8 -*-

import os, datetime, sys, traceback, csv, pickle
import Kross, KPlato


class BusyinfoExporter:

    def __init__(self, scriptaction):
        self.scriptaction = scriptaction
        self.currentpath = self.scriptaction.currentPath()

        self.proj = KPlato.project()
        
        self.forms = Kross.module("forms")
        self.dialog = self.forms.createDialog(self.forms.tr("Busy Information Export"))
        self.dialog.setButtons("Ok|Cancel")
        self.dialog.setFaceType("List") #Auto Plain List Tree Tabbed

        datapage = self.dialog.addPage(self.forms.tr("Schedules"),self.forms.tr("Export Selected Schedule"),"document-export")
        self.scheduleview = KPlato.createScheduleListView(datapage)
        
        savepage = self.dialog.addPage(self.forms.tr("Save"),self.forms.tr("Export Busy Info File"),"document-save")
        self.savewidget = self.forms.createFileWidget(savepage, "kfiledialog:///kplatobusyinfoexportsave")
        self.savewidget.setMode("Saving")
        self.savewidget.setFilter("*.rbi|%(1)s\n*|%(2)s" % { '1' : self.forms.tr("Resource Busy Information"), '2' : self.forms.tr("All Files") } )

        if self.dialog.exec_loop():
            try:
                self.doExport( self.proj )
            except Exception, inst:
                self.forms.showMessageBox("Error", self.forms.tr("Error"), "%s" % inst)
            except:
                self.forms.showMessageBox("Error", self.forms.tr("Error"), "%s" % "".join( traceback.format_exception(sys.exc_info()[0],sys.exc_info()[1],sys.exc_info()[2]) ))

    def doExport( self, project ):
        filename = self.savewidget.selectedFile()
        if not filename:
            raise Exception, self.forms.tr("No file selected")
        schId = self.scheduleview.currentSchedule()
        if schId == -1:
            raise Exception, self.forms.tr("No schedule selected")
        file = open( filename, 'w' )
        p = []
        p.append( project.id() )
        p.append( KPlato.data( project, 'NodeName' ) )
        pickle.dump( p, file )
        for i in range( project.resourceGroupCount() ):
            g = project.resourceGroupAt( i )
            for ri in range( g.resourceCount() ):
                r = g.resourceAt( ri )
                lst = r.appointmentIntervals( schId )
                for iv in lst:
                    iv.insert( 0, r.id() )
                    iv.insert( 1, KPlato.data( r, 'ResourceName' ) )
                    pickle.dump( iv, file )

        file.close()

BusyinfoExporter( self )
