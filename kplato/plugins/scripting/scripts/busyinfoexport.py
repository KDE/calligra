#!/usr/bin/env kross
# -*- coding: utf-8 -*-

import os, datetime, sys, traceback, csv, pickle
import Kross, KPlato

T = Kross.module("kdetranslation")

def i18n(text, args = []):
    if T is not None:
        return T.i18n(text, args).decode('utf-8')
    # No translation module, return the untranslated string
    for a in range( len(args) ):
        text = text.replace( ("%" + "%d" % ( a + 1 )), str(args[a]) )
    return text

class BusyinfoExporter:

    def __init__(self, scriptaction):
        self.scriptaction = scriptaction
        self.currentpath = self.scriptaction.currentPath()

        self.proj = KPlato.project()
        
        self.forms = Kross.module("forms")
        self.dialog = self.forms.createDialog(i18n("Busy Information Export"))
        self.dialog.setButtons("Ok|Cancel")
        self.dialog.setFaceType("List") #Auto Plain List Tree Tabbed

        datapage = self.dialog.addPage(i18n("Schedules"),i18n("Export Selected Schedule"),"document-export")
        self.scheduleview = KPlato.createScheduleListView(datapage)
        
        savepage = self.dialog.addPage(i18n("Save"),i18n("Export Busy Info File"),"document-save")
        self.savewidget = self.forms.createFileWidget(savepage, "kfiledialog:///kplatobusyinfoexportsave")
        self.savewidget.setMode("Saving")
        self.savewidget.setFilter("*.rbi|%(1)s\n*|%(2)s" % { '1' : i18n("Resource Busy Information"), '2' : i18n("All Files") } )

        if self.dialog.exec_loop():
            try:
                self.doExport( self.proj )
            except Exception, inst:
                self.forms.showMessageBox("Sorry", i18n("Error"), "%s" % inst)
            except:
                self.forms.showMessageBox("Error", i18n("Error"), "%s" % "".join( traceback.format_exception(sys.exc_info()[0],sys.exc_info()[1],sys.exc_info()[2]) ))

    def doExport( self, project ):
        filename = self.savewidget.selectedFile()
        if not filename:
            raise Exception, i18n("No file selected")
        schId = self.scheduleview.currentSchedule()
        if schId == -1:
            raise Exception, i18n("No schedule selected")
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
