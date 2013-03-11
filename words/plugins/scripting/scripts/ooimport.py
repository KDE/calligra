#!/usr/bin/env kross
# -*- coding: utf-8 -*-

"""
This python script connects Words and OpenOffice.org together.

(C)2006 Sebastian Sauer <mail@dipe.org>

http://kross.dipe.org
http://www.calligra.org/words
http://udk.openoffice.org/python/python-bridge.html

Dual-licensed under LGPL v2+higher and the BSD license.
"""

import sys, os, popen2, time, traceback, tempfile
import Kross, Words

class Importer:
    def __init__(self, action):
        self.action = action
        self.forms = Kross.module("forms")
        self.pyfile = os.path.join(action.currentPath(), 'oouno.py')
        if not os.path.isfile(self.pyfile):
            raise "Failed to find the required oouno.py file."

    def showDialog(self):
        dialog = self.forms.createDialog("Import file with OpenOffice.org")
        dialog.setButtons("Ok|Cancel")
        dialog.setFaceType("List") #Auto Plain List Tree Tabbed

        openpage = dialog.addPage("Open","Import File","document-open")
        startDirOrVariable = "kfiledialog:///wordsooimport"
        openwidget = self.forms.createFileWidget(openpage, startDirOrVariable)
        openwidget.setMode("Opening")
        filters = [
            "*.odt|OpenDocument Text (*.odt)",
            "*.ott|OpenDocument Text Template (*.ott)",
            "*.sxw|OpenOffice.org 1.0 Text Document (*.sxw)",
            "*.stw|OpenOffice.org 1.0 Text Document Template (*.stw)",
            "*.doc|Microsoft Word 95/97/2000/XP (*.doc)",
            "*.dot|Microsoft Word 95/97/2000/XP Template (*.dot)",
            "*.xml|Microsoft Word 2003 XML (*.xml)",
            "*.rtf|Rich Text Format (*.rtf)",
            "*.txt|Text (*.txt)",
            "*.html *.htm|HTML Document (*.html *.htm)",
            "*.xml|DocBook (*.xml)",
            "*.sdw|StarWriter 1.0 - 5.0 (*.sdw)",
            "*.vor|StarWriter 3.0 - 5.0 Templates (*.vor)",
            "*.wpd|WordPerfect Document (*.wpd)",
            "*.lwp|Lotus WordPro Document (*.lwp)",
            "*.jtd|Ichitaro 8/9/10/11 (*.jtd)",
            "*.jtt|Ichitaro 8/9/10/11 Template (*.jtt)",
            "*.hwp|Hangul WP 97 (*.hwp)",
            "*.wps|WPS 2000/Office 1.0 (*.wps)",
        ]
        filters.insert(0, "%s|All Supported Files" % " ".join( [f.split("|")[0] for f in filters] ))
        filters.append("*|All Files")
        openwidget.setFilter("\n".join(filters))

        configpage = dialog.addPage("Connect","OpenOffice.org UNO Connection","network-server")
        configwidget = self.forms.createWidgetFromUIFile(configpage, os.path.join(self.action.currentPath(),"ooimportconfig.ui"))

        importpage = dialog.addPage("Options","Import Options","preferences-other")
        importwidget = self.forms.createWidgetFromUIFile(importpage, os.path.join(self.action.currentPath(),"ooimport.ui"))

        while True:
            if not dialog.exec_loop():
                #dialog.delayedDestruct()
                return None
            loadfile = openwidget.selectedFile()
            if loadfile and os.path.isfile(loadfile):
                break
            self.forms.showMessageBox("Error","Error","No such file \"%s\"" % loadfile)

        options = {}
        options["loadfile"] = loadfile
        options["host"] = configwidget["hostEdit"].text
        options["port"] = configwidget["portEdit"].value
        if configwidget["timeoutCheckBox"].checked:
            options["timeout"] = configwidget["timeoutEdit"].value
        else:
            options["timeout"] = -1
        options["startupServer"] = configwidget["startupCheckBox"].checked
        options["hideClient"] = configwidget["hideClientCheckBox"].checked
        options["autoCloseDocument"] = configwidget["autoCloseCheckBox"].checked

        if importwidget["odtRadioButton"].checked:
            options["_savefileext"] = '.odt'
        elif importwidget["textRadioButton"].checked:
            options["_savefileext"] = '.txt'
        elif importwidget["htmlRadioButton"].checked:
            options["_savefileext"] = '.html'
        else:
            raise "Invalid option for the intermedia format"

        return options

    def doImport(self, options):
        savefile = tempfile.mktemp()
        savefile += options["_savefileext"]
        options["savefile"] = savefile

        progress = self.forms.showProgressDialog("Import...", options["loadfile"])
        progress.setRange(0,150)
        progress.setValue(0)
        canceled = False
        step = 0
        try:
            args = ["python", self.pyfile]
            args += [ "--%s=%s" % (n,options[n]) for n in options.keys() if not n.startswith('_') ]
            print "Execute: ", args, "\n"
            proc = popen2.Popen3( args, capturestderr = 1 )
            errmsg = proc.childerr.read()
            if errmsg:
                progress.addText("<b>%s</b>" % errmsg)
            fromchild = proc.fromchild.readline()
            while fromchild:
                step += 1
                progress.setValue(step)
                if fromchild and fromchild.strip() != "":
                    progress.addText(fromchild)
                sys.stdout.write( fromchild )
                sys.stdout.flush()

                errmsg = proc.childerr.read()
                if errmsg:
                    progress.addText("<b>%s</b>" % errmsg)
                canceled = progress.isCanceled()
                if canceled:
                    break
                #time.sleep(1)
                fromchild = proc.fromchild.readline()

            if not canceled:
                if not os.path.isfile(savefile):
                    if not proc.childerr:
                        progress.addText("<b>Error: Seems OpenOffice.org failed to write the intermedia file \"%s\"</b>" % savefile)
                    canceled = True
                else:
                    progress.addText("Words imports the intermedia file \"%s\" now" % savefile)
                    self.importFile(savefile, options)
        finally:
            try:
                if os.path.isfile(savefile):
                    os.remove(savefile)
            except:
                pass
            if canceled:
                progress.addText("<br><b>Import aborted.</b>")
            else:
                progress.addText("<br><b>Import successful finished.</b>")
            progress.exec_loop()

        #if self.dialog.exec_loop():
            #try:
                #file = openwidget.selectedFile()
                #if not file or not os.path.isfile(file):
                    #raise "No such file: %s" % file
                #progressThread = ProgressThread(self.forms, file)
                #progressThread.start()
                #try:
                    #controller = UnoController()
                    #progressThread.value = 5
                    #controller.connect()
                    #try:
                        #progressThread.value = 30
                        #controller.loadDocument( "file://%s" % file )
                        #progressThread.value = 70
                        #outputstream = WordsOutputStream(controller.unoConfig)
                        #controller.writeDocument(outputstream)
                        #progressThread.value = 90
                        #outputstream.flush()
                    #finally:
                        #controller.disconnect()
                #finally:
                    #progressThread.finish()
                    #progressThread.join() # wait till the thread finished
            #except:
                #tb = "".join( traceback.format_exception(sys.exc_info()[0],sys.exc_info()[1],sys.exc_info()[2]) )
                #self.forms.showMessageBox("Error","Error","%s" % tb)

    def importFile(self, fromFileName, options):
        fs = Words.mainFrameSet()
        if not fs:
            raise "No main frameset to import the content too."
        doc = fs.document()
        if not fs:
            raise "No document to import the content too."

        fileExt = os.path.splitext(fromFileName)[1].lower()
        if fileExt == '.txt' or fileExt == '.html':
            f = open(fromFileName, "r")
            if fileExt == '.txt':
                doc.setText( ''.join(f.readlines()) )
            else:
                doc.setHtml( ' '.join(f.readlines()) )
            f.close()
        else: #odt

            #TODO this crashes horrible :-(
            Words.document().openUrl(fromFileName)

def start(action):
    importer = Importer(action)
    options = importer.showDialog()
    if options != None:
        importer.doImport(options)

start(self)

##print "CONNECT ..."
##controller.connect()
##print "LOADING DOC ..."
##controller.loadDocument("file:///home/kde4/bla2.odt")
###controller.loadDocument("file:///home/kde4/bla.odt")
##print "DUMP DOC ..."
##controller.writeDocument()
##print "DISCONNECT ..."
##controller.disconnect()
##print "DONE"


