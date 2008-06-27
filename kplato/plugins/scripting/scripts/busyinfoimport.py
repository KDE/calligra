#!/usr/bin/env kross
# -*- coding: utf-8 -*-

import os, datetime, sys, traceback, pickle
import Kross, KPlato


class BusyinfoImporter:

    def __init__(self, scriptaction):
        self.scriptaction = scriptaction
        self.currentpath = self.scriptaction.currentPath()

        self.proj = KPlato.project()
        
        self.forms = Kross.module("forms")
        self.dialog = self.forms.createDialog("Busy Information Import")
        self.dialog.setButtons("Ok|Cancel")
        self.dialog.setFaceType("List") #Auto Plain List Tree Tabbed

        openpage = self.dialog.addPage("Open","Import Busy Info File","document-open")
        self.openwidget = self.forms.createFileWidget(openpage, "kfiledialog:///kplatobusyinfoimportopen")
        self.openwidget.setMode("Opening")
        self.openwidget.setFilter("*.rbi|Resource Busy Information\n*|All Files")

        if self.dialog.exec_loop():
            try:
                self.doImport( self.proj )
            except:
                self.forms.showMessageBox("Error", "Error", "%s" % "".join( traceback.format_exception(sys.exc_info()[0],sys.exc_info()[1],sys.exc_info()[2]) ))

    def doImport( self, project ):
        filename = self.openwidget.selectedFile()
        if not os.path.isfile(filename):
            raise "File '%s' not found." % filename

        file = open(filename,'r')
        try:
            # load project id and -name
            data = pickle.load( file )
            print data
            if not self.validProject( project, data ):
                raise "Invalid project: %s, %s" % ( data[0], data[1] )
            pid = data[0]
            pname = data[1]
            # clear existing, so we don't get double up
            project.clearExternalAppointments( pid )
            # load the intervals
            while True:
                data = pickle.load( file )
                self.loadAppointment( project, pid, pname, data )

        except:
            file.close()

    def validProject( self, project, data ):
        if project.id() == data[0]:
            #TODO warning?
            print "Trying to load data from my own project"
            return False
        return True

    def loadAppointment( self, project, pid, pname, data ):
        r = project.findResource( data[0] )
        if r is None:
            print "Resource is not used in this project: %s, %s" % ( data[0], data[1] )
            return
        if KPlato.data( r, 'ResourceName' ) != data[1]:
            #TODO Warning ?
            print "Resources has same id but different names %s - %s" % ( KPlato.data( r, 'ResourceName' ), data[1] )
        r.addExternalAppointment( pid, pname, data[2:5] )

BusyinfoImporter( self )
