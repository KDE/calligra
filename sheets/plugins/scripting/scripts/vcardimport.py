#!/usr/bin/env kross

"""
Python script to import content from a vCard file to Sheets.

(C)2007 Sebastian Sauer <mail@dipe.org>
http://kross.dipe.org
http://www.calligra.org/sheets
Dual-licensed under LGPL v2+higher and the BSD license.
"""

try:
    import vobject
except ImportError:
    raise "Failed to import required python module: VObject - iCalendar and vCard Library"

import os, Kross, KSpread

T = Kross.module("kdetranslation")

class MyConfig:
    def __init__(self):
        self.validate=False
        self.transform=True
        self.findBegin=True
        self.ignoreUnreadable=True

class MyDialog:
    def __init__(self, action, config):
        self.action = action
        self.config = config

        forms = Kross.module("forms")
        dialog = forms.createDialog(T.i18n("Import vCard"))
        #dialog.minimumWidth = 400
        #dialog.minimumHeight = 300
        dialog.setButtons("Ok|Cancel")
        dialog.setFaceType("List") #Auto Plain List Tree Tabbed
        
        filepage = dialog.addPage(T.i18n("Open"), T.i18n("vCard File"), "document-open")
        #deffile = "/home/kde4/vcardtest/addressbook.vcf" #"kfiledialog:///kspreadvcardimport"
        deffile = "kfiledialog:///kspreadvcardimport"
        self.openwidget = forms.createFileWidget(filepage, deffile)
        self.openwidget.setMode("Opening")
        self.openwidget.setFilter("*.vcf|%(1)s\n*|%(2)s" % { '1' : T.i18n("vCard Files"), '2' : T.i18n("All Files") } )
        
        datapage = dialog.addPage(T.i18n("Import"),T.i18n("Import to sheet beginning at cell"),"document-import")
        self.sheetslistview = KSpread.createSheetsListView(datapage)
        self.sheetslistview.setEditorType("Cell")

        if not dialog.exec_loop():
            return

        vcardfilename = self.openwidget.selectedFile()
        if not os.path.isfile(vcardfilename):
            raise Exception, T.i18n("File '%1' not found.", [vcardfilename])

        currentSheet = self.sheetslistview.sheet()
        if not currentSheet:
            raise Exception, T.i18n("No current sheet.")

        self.importVCardFile(vcardfilename, currentSheet)

    def importVCardFile(self, vcardfilename, currentSheet):
        self.componentDict = {}
        self.componentList = []
        
        writer = KSpread.writer()
        if not writer.setSheet(currentSheet):
            raise Exception, T.i18n("Invalid sheet \"%1\" defined.", [currentSheet])

        writer.next() #hack
        writer.next() #hack

        f = open(vcardfilename, 'r')
        lines = []
        for line in f:
            if not line.strip() == "":
                lines.append( line )
                continue
            if len(lines) > 0:
                self.importVCardLines(writer, lines)
                lines = []
        if len(lines) > 0:
            self.importVCardLines(writer, lines)

        if writer.setSheet(currentSheet):
            writer.next() #hack
            writer.setValues(self.componentList)

    def importVCardLines(self, writer, lines = []):  
        try:
            vcard = vobject.readOne("\n".join(lines), self.config.validate, self.config.transform, self.config.findBegin, self.config.ignoreUnreadable)
            self.importVCard(writer, vcard)
        except StopIteration:
            pass

    def importVCard(self, writer, vcard):
        print ">>>>>>>>>>>> %s" % vcard.name
        #print "cal.behavior = %s" % cal.behavior #<class 'vobject.vcard.VCard3_0'>
        #print "cal.components = %s" % cal.components #<bound method Component.components of <VCARD| [<VERSION{}3.0>, <UID{}gO2CjZKrWx>, <CLASS{}PUBLIC>, <FN{}Other Name>, <N{} Other Contact Name >, <ORG{}My Other Orgnaization>]>>
        #print "cal.name = %s" % cal.name #"VCARD"
        #print "cal.lines = %s" % cal.lines #<bound method Component.lines of <VCARD| [<VERSION{}3.0>, <UID{}gO2CjZKrWx>, <CLASS{}PUBLIC>, <FN{}Other Name>, <N{} Other Contact Name >, <ORG{}My Other Orgnaization>]>>

        components = []
        for c in vcard.getChildren():
            components.append(c)
        componentNames = {}
        for idx in range(0, len(components)):
            n = components[idx].name
            componentNames[n] = idx
        
        record = []
        for n in self.componentList:
            if componentNames.has_key(n):
                idx = componentNames[n]
                record.append("%s" % components[idx].value)
            else:
                record.append("")
        
        for idx in range(0,len(components)):
            n = components[idx].name
            if not self.componentDict.has_key(n):
                self.componentDict[n] = len(self.componentList)
                self.componentList.append(n)
                record.append("%s" % components[idx].value)
        
        print record
        
        #for component in vcard.getChildren():
            #print "component.name = %s" % component.name 
            #print "component.group = %s" % component.group #None
            #print "component.params = %s" % component.params #{}
            #print "component.serialize = %s" % component.serialize()
            #print "component.value = %s" % component.value
            #print dir(component) #['autoBehavior', 'behavior', 'clearBehavior', 'copy', 'duplicate', 'encoded', 'getChildren', 'group', 'isNative', 'lineNumber', 'name', 'params', 'parentBehavior', 'prettyPrint', 'serialize', 'setBehavior', 'singletonparams', 'transformChildrenFromNative', 'transformChildrenToNative', 'transformFromNative', 'transformToNative', 'validate', 'value', 'valueRepr']

        if not writer.setValues(record):
            print "Failed to set all of '%s' to cell '%s'" % (record,writer.cell())
            return

        writer.next()

myconfig = MyConfig()
MyDialog(self, myconfig)
