#!/usr/bin/env kross
# -*- coding: utf-8 -*-

import os, datetime, sys, traceback, pickle
import Kross, Plan

T = Kross.module("kdetranslation")

class BusyinfoImporter:

    def __init__(self, scriptaction):
        self.scriptaction = scriptaction
        self.currentpath = self.scriptaction.currentPath()

        self.proj = Plan.project()
        
        self.forms = Kross.module("forms")
        self.dialog = self.forms.createDialog(T.i18n("Busy Information Import"))
        self.dialog.setButtons("Ok|Cancel")
        self.dialog.setFaceType("List") #Auto Plain List Tree Tabbed

        openpage = self.dialog.addPage(T.i18n("Open"), T.i18n("Import Busy Info File"),"document-open")
        self.openwidget = self.forms.createFileWidget(openpage, "kfiledialog:///kplatobusyinfoimportopen")
        self.openwidget.setMode("Opening")
        self.openwidget.setFilter("*.rbi|%(1)s\n*|%(2)s" % { '1' : T.i18n("Resource Busy Information"), '2' : T.i18n("All Files") } )

        if self.dialog.exec_loop():
            try:
                Plan.beginCommand( T.i18nc("(qtundo_format)", "Import resource busy information") )
                self.doImport( self.proj )
                Plan.endCommand()
            except:
                Plan.revertCommand()
                self.forms.showMessageBox("Error", T.i18n("Error"), "%s" % "".join( traceback.format_exception(sys.exc_info()[0],sys.exc_info()[1],sys.exc_info()[2]) ))

    def doImport( self, project ):
        filename = self.openwidget.selectedFile()
        if not os.path.isfile(filename):
            self.forms.showMessageBox("Sorry", T.i18n("Error"), T.i18n("No file selected") )
            return

        file = open(filename,'r')
        try:
            # load project id and -name
            data = pickle.load( file )
            #print data
            pid = data[0]
            if project.id() == pid:
                self.forms.showMessageBox("Error", T.i18n("Error"), T.i18n("Cannot load data from project with the same identity") )
                raise Exception
            pname = data[1].decode( "UTF-8" )
            # clear existing, so we don't get double up
            project.clearExternalAppointments( pid )
            # load the intervals
            while True:
                data = pickle.load( file )
                self.loadAppointment( project, pid, pname, data )

        except:
            file.close()

    def loadAppointment( self, project, pid, pname, data ):
        r = project.findResource( data[0] )
        if r is None:
            print "Resource is not used in this project: %s, %s" % ( data[0], data[1] )
            return
        if project.data( r, 'ResourceName' ) != data[1]:
            #TODO Warning ?
            print "Resources has same id but different names %s - %s" % ( project.data( r, 'ResourceName' ), data[1] )
        r.addExternalAppointment( pid, pname, data[2:5] )

BusyinfoImporter( self )
