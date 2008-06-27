#!/usr/bin/env kross
# -*- coding: utf-8 -*-

import os, datetime, sys, traceback, pickle
import Kross, KPlato


class ResourcesImporter:

    def __init__(self, scriptaction):
        self.scriptaction = scriptaction
        self.currentpath = self.scriptaction.currentPath()

        self.proj = KPlato.project()
        
        self.forms = Kross.module("forms")
        self.dialog = self.forms.createDialog("Resources Import")
        self.dialog.setButtons("Ok|Cancel")
        self.dialog.setFaceType("List") #Auto Plain List Tree Tabbed

        #TODO add options page ( import Calendars? Select calendars, Select resources... )
        
        openpage = self.dialog.addPage("Open","KPlato Resources","document-open")
        self.openwidget = self.forms.createFileWidget(openpage, "kfiledialog:///kplatresourcesimportopen")
        self.openwidget.setMode("Opening")
        self.openwidget.setFilter("*.kplato|KPlato Project Files\n*|All Files")

        if self.dialog.exec_loop():
            try:
                self.doImport( self.proj )
            except:
                self.forms.showMessageBox("Error", "Error", "%s" % "".join( traceback.format_exception(sys.exc_info()[0],sys.exc_info()[1],sys.exc_info()[2]) ))

    def doImport( self, project ):
        filename = self.openwidget.selectedFile()
        if not os.path.isfile(filename):
            raise "File '%s' not found." % filename

        Other = KPlato.openDocument("Other", filename)
        if Other is None:
            raise "Could not open document %s" % filename
        otherproj = Other.project()
        if otherproj is None:
            raise "No project!"
        if project.id() == otherproj.id():
            #FIXME: when project ids are unique
            #raise "Project identity identical"
            print "Project identity identical: %s %s" % ( KPlato.data( project, 'NodeName' ), Other.data( otherproj, 'NodeName' ) )

        for ci in range( otherproj.calendarCount() ):
            self.doImportCalendar( project, otherproj.calendarAt( ci ) )
        #TODO Default calendar
    
        for gi in range( otherproj.resourceGroupCount() ):
            othergroup = otherproj.resourceGroupAt( gi )
            gr = project.findResourceGroup( othergroup.id() )
            if gr is None:
                gr = project.createResourceGroup( othergroup )
                if gr is None:
                    raise "Unable to create copy of resource group: " % ( othergroup.id() )
            for ri in range( othergroup.resourceCount() ):
                otherresource = othergroup.resourceAt( ri )
                if otherresource is None:
                    raise "No resource to copy from"
                self.doImportResource( project, gr, otherresource )

    def doImportResource( self, project, group, resource ):
        r = project.findResource( resource.id() )
        if r is None:
            r = project.createResource( group, resource )
            if r is None:
                print "Unable to create copy of resource:", r
        else:
            #TODO update?
            print "Resource already exists: %s %s" % ( r.id(), KPlato.data( r, 'ResourceName' ) )
    
    def doImportCalendar( self, project, calendar, parent = None ):
        cal = project.findCalendar( calendar.id() )
        if cal is not None:
            #TODO let user decide
            raise "Calendar already exists: %s" % cal-id()
        # python doesn't seem to give a 0 pointer for a None object
        if parent is None:
            cal = project.createCalendar( calendar )
        else:
            cal = project.createCalendar( calendar, parent )
        if cal is None:
            raise "Unable to create copy of calendar: %s" % ( calendar.id() )
        for ci in range( calendar.childCount() ):
            self.doImportCalendar( project, calendar.childAt( ci ), cal )

ResourcesImporter( self )
