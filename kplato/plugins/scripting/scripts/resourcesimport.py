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
        self.dialog = self.forms.createDialog(self.forms.tr("Resources Import"))
        self.dialog.setButtons("Ok|Cancel")
        self.dialog.setFaceType("List") #Auto Plain List Tree Tabbed

        #TODO add options page ( import Calendars? Select calendars, Select resources... )
        
        openpage = self.dialog.addPage(self.forms.tr("Open"),self.forms.tr("KPlato Resources"),"document-open")
        self.openwidget = self.forms.createFileWidget(openpage, "kfiledialog:///kplatresourcesimportopen")
        self.openwidget.setMode("Opening")
        self.openwidget.setFilter("*.kplato|%(1)s\n*|%(2)s" % { '1' : self.forms.tr("Resource Busy Information"), '2' : self.forms.tr("All Files") } )

        if self.dialog.exec_loop():
            try:
                self.doImport( self.proj )
            except Exception, inst:
                self.forms.showMessageBox("Error", self.forms.tr("Error"), "%s" % inst)
            except:
                self.forms.showMessageBox("Error", self.forms.tr("Error"), "%s" % "".join( traceback.format_exception(sys.exc_info()[0],sys.exc_info()[1],sys.exc_info()[2]) ))

    def doImport( self, project ):
        filename = self.openwidget.selectedFile()
        if not os.path.isfile(filename):
            raise Exception, self.forms.tr("File not found: %(filename)s") % { 'filename' : filename }

        Other = KPlato.openDocument("Other", filename)
        if Other is None:
            raise Exception, self.forms.tr("Could not open document: %(filename)s") % { 'filename' : filename }
        otherproj = Other.project()
        if otherproj is None:
            raise Exception, self.forms.tr("No project to import from")
        if project.id() == otherproj.id():
            raise Exception, self.forms.tr("Project identities are identical")

        for ci in range( otherproj.calendarCount() ):
            self.doImportCalendar( project, otherproj.calendarAt( ci ) )
        #TODO Default calendar

        for gi in range( otherproj.resourceGroupCount() ):
            othergroup = otherproj.resourceGroupAt( gi )
            gr = project.findResourceGroup( othergroup.id() )
            if gr is None:
                gr = project.createResourceGroup( othergroup )
                if gr is None:
                    raise Exception, self.forms.tr("Unable to create copy of resource group: %(name)s") % { 'name' : othergroup.name() }
            for ri in range( othergroup.resourceCount() ):
                otherresource = othergroup.resourceAt( ri )
                if otherresource is None:
                    raise Exception, self.forms.tr("No resource to copy from")
                self.doImportResource( project, gr, otherresource )

    def doImportResource( self, project, group, resource ):
        r = project.findResource( resource.id() )
        if r is None:
            r = project.createResource( group, resource )
            if r is None:
                raise Exception, self.forms.tr("Unable to create copy of resource: %(name)s") % { 'name' : resource.name() }
        else:
            #TODO update?
            print "Resource already exists: %s %s" % ( r.id(), KPlato.data( r, 'ResourceName' ) )
    
    def doImportCalendar( self, project, calendar, parent = None ):
        cal = project.findCalendar( calendar.id() )
        if cal is not None:
            #TODO let user decide
            raise Exception, "Calendar already exists: %(name)s" % { 'name' : cal.name() }
        # python doesn't seem to give a 0 pointer for a None object
        if parent is None:
            cal = project.createCalendar( calendar )
        else:
            cal = project.createCalendar( calendar, parent )
        if cal is None:
            raise Exception, "Unable to create copy of calendar: %(name)s" % { 'name' : calendar.name() }
        for ci in range( calendar.childCount() ):
            self.doImportCalendar( project, calendar.childAt( ci ), cal )

ResourcesImporter( self )
