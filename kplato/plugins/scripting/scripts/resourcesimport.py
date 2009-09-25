#!/usr/bin/env kross
# -*- coding: utf-8 -*-

import os, datetime, sys, traceback, pickle
import Kross, KPlato

T = Kross.module("kdetranslation")
def i18n(text, args = []):
    if T is not None:
        return T.i18n(text, args).decode('utf-8')
    # No translation module, return the untranslated string
    for a in range( len(args) ):
        text = text.replace( ("%" + "%d" % ( a + 1 )), str(args[a]) )
    return text

class ResourcesImporter:

    def __init__(self, scriptaction):
        self.scriptaction = scriptaction

        self.proj = KPlato.project()
        
        self.forms = Kross.module("forms")
        self.dialog = self.forms.createDialog(i18n("Resources Import"))
        self.dialog.setButtons("Ok|Cancel")
        self.dialog.setFaceType("List") #Auto Plain List Tree Tabbed

        #TODO add options page ( import Calendars? Select calendars, Select resources... )
        
        openpage = self.dialog.addPage(i18n("Open"),i18n("KPlato Resources"),"document-open")
        self.openwidget = self.forms.createFileWidget(openpage, "kfiledialog:///kplatresourcesimportopen")
        self.openwidget.setMode("Opening")
        self.openwidget.setFilter("*.kplato|%(1)s\n*|%(2)s" % { '1' : i18n("Resource Busy Information"), '2' : i18n("All Files") } )

        if self.dialog.exec_loop():
            try:
                self.doImport( self.proj )
            except:
                self.forms.showMessageBox("Error", i18n("Error"), "%s" % "".join( traceback.format_exception(sys.exc_info()[0],sys.exc_info()[1],sys.exc_info()[2]) ))

    def doImport( self, project ):
        filename = self.openwidget.selectedFile()
        if not os.path.isfile(filename):
            raise Exception, i18n("No file selected")

        Other = KPlato.openDocument("Other", filename)
        if Other is None:
            self.forms.showMessageBox("Sorry", i18n("Error"), i18n("Could not open document: %1", [filename]))
            return
        otherproj = Other.project()
        if otherproj is None:
            self.forms.showMessageBox("Sorry", i18n("Error"), i18n("No project to import from"))
            return
        if project.id() == otherproj.id():
            self.forms.showMessageBox("Sorry", i18n("Error"), i18n("Project identities are identical"))
            return

        for ci in range( otherproj.calendarCount() ):
            self.doImportCalendar( project, otherproj.calendarAt( ci ) )
        #TODO Default calendar

        for gi in range( otherproj.resourceGroupCount() ):
            othergroup = otherproj.resourceGroupAt( gi )
            gr = project.findResourceGroup( othergroup.id() )
            if gr is None:
                gr = project.createResourceGroup( othergroup )
                if gr is None:
                    self.forms.showMessageBox("Sorry", i18n("Error"), i18n("Unable to create copy of resource group: %1", [othergroup.name()]))
                    return
            for ri in range( othergroup.resourceCount() ):
                otherresource = othergroup.resourceAt( ri )
                if otherresource is None:
                    self.forms.showMessageBox("Sorry", i18n("Error"), i18n("No resource to copy from"))
                    return
                self.doImportResource( project, gr, otherresource )

    def doImportResource( self, project, group, resource ):
        r = project.findResource( resource.id() )
        if r is None:
            r = project.createResource( group, resource )
            if r is None:
                self.forms.showMessageBox("Sorry", i18n("Error"), i18n("Unable to create copy of resource: %1", [resource.name()]))
                return
        else:
            #TODO update?
            print "Resource already exists: %s %s" % ( r.id(), KPlato.data( r, 'ResourceName' ) )
    
    def doImportCalendar( self, project, calendar, parent = None ):
        cal = project.findCalendar( calendar.id() )
        if cal is not None:
            #TODO let user decide
            self.forms.showMessageBox("Sorry", i18n("Error"), i18n("Calendar already exists: %1", [cal.name()]))
            return
        # python doesn't seem to give a 0 pointer for a None object
        if parent is None:
            cal = project.createCalendar( calendar )
        else:
            cal = project.createCalendar( calendar, parent )
        if cal is None:
            self.forms.showMessageBox("Sorry", i18n("Error"), i18n("Unable to create copy of calendar: %1", [calendar.name()]))
            return
        for ci in range( calendar.childCount() ):
            self.doImportCalendar( project, calendar.childAt( ci ), cal )

ResourcesImporter( self )
