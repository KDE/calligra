#!/usr/bin/env krossrunner

class Styles:

    Simple = (
        "html { background-color:#ffffff; color:#000; }"
        "body { margin:1em; }"
    )

    Paper = (
        "html { background-color:#efefef; }"
        "body { background-color:#fafafa; color:#303030; margin:1em; padding:1em; border:#606060 1px solid; }"
        "h1 { text-align:center; }"
        "tr.row1 { background-color:#ffefef; }"
        "tr.row0 { background-color:#fff0ff; }"
    )

    Desert = (
        "html { background-color:#fffff0; font-family: \"courier new\", courier, monospace; }"
        "body { background-color:#fffff0; color:#660000; margin:1em; padding:1em; }"
        "h1 { color:#660000; text-align:center; }"
        "th { padding:0.3em; background-color:#efefef; }"
        "td { padding:0.3em; }"
        "tr.row1 { background-color:#efffef; }"
        "tr.row0 { background-color:#ffefef; }"
    )

    SeaWater = (
        "html { background-color:#0000aa; }"
        "body { background-color:#000066; color:#efefff; margin:1em; padding:1em; border:#00f 1px solid; }"
        "h1 { color:#0000ff; }"
        "th { color:#6666ff; }"
        "h1 { color:#0000ff; text-align:center; }"
        "th { border:#00f 1px solid; color:#6666ff; padding:0.2em; }"
        "table { border:#00f 1px solid; padding:1em; }"
        "tr.row1 { background-color:#000060; }"
        "tr.row0 { background-color:#00003f; }"
        "td { border:#00f 1px solid; padding:0.2em; }"
    )

    def __init__(self):
        self._currentRow = 3 #Simple is default
        self._items = [ s for s in dir(Styles) if not s.startswith('_') ]
        self._uiItems = ''.join( [ '<item><property name="text" ><string>%s</string></property></item>' % s for s in self._items ] )

class Reader:

    class Static:
        def __init__(self):
            import datetime
            self.filename = ''
            self.infos = {
                'Title' : 'Some Title',
                'Subject' : 'Some Subject',
                'Author' : 'Tester',
                'EMail' : 'Tester@nowhere.test',
                'Keywords' : 'keyword1 keyword2 keyword3',
                'Filename' : "/home/bla/test.ods",
                'Date' : datetime.datetime.now().strftime("%Y-%m-%d %H:%M"),
            }
        def hasFile(self):
            return True
        def setFile(self, filename):
            self.filename = filename
        def openFile(self, progress = None):
            self.records = [
                ['A1','A2','A3','A4'],
                ['B1','B2','B3','B4'],
                ['C1','C2','C3','C4'],
            ]
            self.recordidx = 0
        def closeFile(self):
            pass
        def readRecord(self):
            if self.recordidx < len(self.records):
                record = self.records[ self.recordidx ]
                self.recordidx += 1
                return record
            return None

    class File:
        def __init__(self):
            self.embeddedInKSpread = False
            try:
                import KSpread
                self.kspread = KSpread
                self.embeddedInKSpread = True
            except ImportError:
                try:
                    import Kross
                    self.kspread = Kross.module("kspread")
                except ImportError:
                    raise "Failed to import the Kross module. Please run this script with \"kross thisscriptfile.py\""

            application = self.kspread.application()
            self.document = self.kspread.document()

            import datetime

            # default filename is empty
            self.filename = ''
            # use following for testing to access direct the OpenDocument Spreadsheet File without saveas-dialog.
            #self.filename = '/home/kde4/kspreaddocument.ods'

            if self.embeddedInKSpread:
                pass
            elif self.filename and self.filename != '':
                self.setFile(self.filename)
                self.openFile()
            elif self.document.url():
                self.setFile(self.document.url())

            self.infos = {
                'Title' : self.document.documentInfoTitle(),
                'Subject' : self.document.documentInfoSubject(),
                'Author' : self.document.documentInfoAuthorName(),
                'EMail' : self.document.documentInfoEmail(),
                'Keywords' : self.document.documentInfoKeywords(),
                'Filename' : self.document.url(),
                'Date' : datetime.datetime.now().strftime("%Y-%m-%d %H:%M"),
            }

            print "Application: %s" % application
            print "Document: %s" % self.document
            #print dir(self.kspread)
            #print dir(application)
            #print dir(document)
            print "Filename: %s" % self.filename
            print "Embedded in KSpread: %s" % self.embeddedInKSpread
            print "Sheetnames: %s" % self.kspread.sheetNames()

        def extractFileFromUrl(self, url):
            import urlparse
            (scheme, netloc, path, params, query, fragment) = urlparse.urlparse(url)
            return path

        def hasFile(self):
            return self.embeddedInKSpread or (self.filename and self.filename != '')

        def setFile(self, filename):
            import os
            path = self.extractFileFromUrl(filename)
            if not os.path.isfile(path):
                raise "No such file \"%s\" to read from." % filename
            self.filename = path

        def openFile(self, progress = None):
            if not self.embeddedInKSpread:
                file = self.extractFileFromUrl( self.filename )
                docfile = self.extractFileFromUrl( self.document.url() )
                if file != docfile:
                    if not self.kspread.openUrl( self.filename ):
                        raise "Failed to open the file \"%s\"." % self.filename

            sheetnames = self.kspread.sheetNames()
            if len(sheetnames) < 1:
                raise "No sheets which could be exported to HTML."

            self.sheetidx = 0
            self.sheet = self.kspread.sheetByName( sheetnames[0] )
            self.rowidx = 0
            print "Reader.openFile file=%s rowidx=%i maxRow=%i maxColumn=%i" % (self.filename, self.rowidx, self.sheet.maxRow(), self.sheet.maxColumn())
            self.progress = progress
            if self.progress:
                self.progress.labelText = "Processing sheet \"%s\"" % sheetnames[0]
                self.progress.maximum = self.sheet.maxRow()

        def closeFile(self):
            pass

        def readRecord(self):
            #FIXME this is not optimal since we need wo walk at least through 256*256 cells. It
            #would be better, if we reuse KSpread::Cell::firstCell()-iterator here.
            if self.rowidx < self.sheet.maxRow():
                record = []
                for i in range(self.sheet.maxColumn(), 0, -1):
                    value = self.sheet.text(i, self.rowidx)
                    #print "col=%i row=%i value=%s" % (i, self.rowidx, value)
                    if value or len(record) > 0:
                        record.insert(0,value)
                self.rowidx += 1
                if self.progress:
                    self.progress.value = self.rowidx
                return record
            print "EXPORT DONE rowidx=%i maxRow=%i maxColumn=%i" % (self.rowidx, self.sheet.maxRow(), self.sheet.maxColumn())
            return None

    def __init__(self):
        #self.impl = Reader.Static()
        self.impl = Reader.File()

    def __getattr__(self, name):
        return getattr(self.impl, name)

class Writer:

    class StdOut:
        def __init__(self):
            self.filename = ''
            self.infos = {}
        def hasFile(self):
            return True
        def setFile(self, filename):
            pass
        def openFile(self):
            pass
        def closeFile(self):
            pass
        def writeRecord(self, record):
            print "%s" % record

    class File:
        def __init__(self):
            self.filename = ''
            self.infos = {}
        def hasFile(self):
            return False
        def setFile(self, filename):
            self.filename = filename
        def openFile(self):
            self.file = None
            self.rowidx = 0
            try:
                self.file = open(self.filename, "w")
            except IOError, (errno, strerror):
                raise "Failed to create HTML file \"%s\":\n%s" % (self.filename,strerror)
            if self.infos.has_key('Title'):
                title = self.infos['Title']
            else:
                title = "Spreadsheet"

            self.file.write( "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n" )
            self.file.write( "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\" \"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">\n" )
            self.file.write( "<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"en\">\n" )
            self.file.write( "<body><head><title>%s</title>" % title )

            if hasattr(self,'styles'):
                global Styles
                self.file.write("<style type=\"text/css\">\n<!--\n")
                stylename = self.styles._items[ self.styles._currentRow ]
                self.file.write( getattr(Styles,stylename) )
                self.file.write("\n//-->\n</style>\n")

            self.file.write( "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />\n" )

            self.file.write( "</head><h1>%s</h1><ul>" % title )
            for s in ['Title','Subject','Author','EMail','Keywords','Filename','Date']:
                try:
                    self.file.write( "<li>%s: %s</li>" % (s,self.infos[s]) )
                except:
                    pass
            self.file.write( "</ul><table border=\"1\">" )
        def closeFile(self):
            if self.file != None:
                self.file.write("</table></body></html>")
                self.file.close()
        def writeRecord(self, record):
            if record != None and len(record) > 0:
                self.file.write("<tr class=\"row%i\">" % (self.rowidx % 2 != 0))
                self.rowidx += 1
                self.file.write("<th>%s</th>" % self.rowidx)
                for value in record:
                    self.file.write("<td>%s</td>" % value)
                self.file.write("</tr>")

    def __init__(self, styles):
        #self.impl = Writer.StdOut()
        self.impl = Writer.File()
        self.impl.styles = styles

    def __getattr__(self, name):
        return getattr(self.impl, name)

class Dialog:
    def __init__(self, exporter):
        import Kross, os
        self.exporter = exporter

        self.forms = Kross.module("forms")
        self.dialog = self.forms.createDialog("Export HTML")
        self.dialog.setButtons("Ok|Cancel")
        self.dialog.setFaceType("List") #Auto Plain List Tree Tabbed

        if not self.exporter.reader.hasFile():
            openpage = self.dialog.addPage("Open","Read from OpenDocument Spreadsheet File","fileopen")
            self.openwidget = self.forms.createFileWidget(openpage, "kfiledialog:///kspreadexphtmlopen")
            self.openwidget.setMode("Opening")
            self.openwidget.setFilter("*.ods|OpenDocument Spreadsheet Files\n*|All Files")

        if not self.exporter.writer.hasFile():
            savepage = self.dialog.addPage("Save","Save to HTML File","filesave")
            self.savewidget = self.forms.createFileWidget(savepage, "kfiledialog:///kspreadexphtmlsave")
            self.savewidget.setMode("Saving")
            self.savewidget.setFilter("*.html *.htm *.xhtml|HTML Documents\n*|All Files")

        infospage = self.dialog.addPage("Infos","HTML Document Informations","messagebox_info")
        self.infoswidget = self.forms.createWidgetFromUIFile(infospage, os.path.join(self.exporter.currentpath, "exporthtmlinfos.ui"))
        for i in self.exporter.reader.infos.keys():
            w = self.infoswidget[i]
            w.setText( self.exporter.reader.infos[i] )

        layoutpage = self.dialog.addPage("Styles","Style of the HTML Document","colorize")
        layoutwidget = self.forms.createWidgetFromUI(layoutpage,
            '<ui version="4.0" >'
            ' <class>Form</class>'
            ' <widget class="QWidget" name="Form" >'
            '  <layout class="QHBoxLayout" >'
            '   <item>'
            '    <widget class="QListWidget" name="List">'
            '     <property name="currentRow"><number>%i</number></property>'
            '     %s'
            '    </widget>'
            '   </item>'
            '  </layout>'
            ' </widget>'
            '</ui>'
            % ( self.exporter.writer.styles._currentRow , self.exporter.writer.styles._uiItems )
        )
        print dir(layoutwidget)
        self.layoutlist = layoutwidget["List"]

        #w = widget #widget["KFileDialog::mainWidget"]
        #print dir(w)
        #for idx in range( len(w) ):
        #    obj = w[ idx:idx ][0]
        #    print "...... idx=%s obj=%s name=%s class=%s" % ( idx , obj , obj.__name__ , obj.__class__ )

    def __del__(self):
        self.dialog.delayedDestruct()

    def show(self):
        import os
        result = self.dialog.exec_loop()
        if result:

            #FIXME this one throws a bad python-error. Handle it!
            #self.writer.infos = {}

            # set reader
            if hasattr(self,"openwidget"):
                self.exporter.reader.setFile( self.openwidget.selectedFile() )

            # set writter
            if hasattr(self,"savewidget"):
                savefilename = str( self.savewidget.selectedFile() )
                if os.path.isfile(savefilename):
                    r = self.forms.showMessageBox("WarningContinueCancel", "Overwrite file?", "The file \"%s\" does already exist. Overwrite the file?" % savefilename)
                    if r != "Continue":
                        raise "Export aborted."
                self.exporter.writer.setFile(savefilename)

            # set informations
            for s in ['Title','Subject','Author','EMail','Keywords','Filename','Date']:
                try:
                    self.exporter.writer.infos[s] = self.infoswidget[s].text
                except:
                    pass

            # set style
            self.exporter.writer.styles._currentRow = self.layoutlist.currentRow

        return result

    def showError(self, message):
        self.forms.showMessageBox("Error", "Error", message)

    def showProgress(self):
        progress = self.forms.showProgressDialog("Exporting...", "Initialize...")
        progress.value = 0
        #progress.labelText = "blaaaaaaaaaaa"
        return progress

class Exporter:

    def __init__(self, scriptaction):
        import os, sys

        try:
            import Kross
        except:
            raise "Failed to import the Kross module."

        self.scriptaction = scriptaction
        self.currentpath = self.scriptaction.currentPath()

        global Styles, Reader, Writer
        self.reader = Reader()
        self.writer = Writer( Styles() )

        #import Kross, os
        #window = Kross.activeWindow()
        #if window == None:
        #    print "Creating new dialog"
        #    window = Kross.createDialog("TestGuiFormDialog")
        #else:
        #    print "Using active window"

        global Dialog
        self.dialog = Dialog(self)
        while True:
            try:
                if self.dialog.show():
                    self.doExport()
                break
            except:
                self.dialog.showError(sys.exc_info()[0])

    def doExport(self):
        progress = self.dialog.showProgress()

        self.reader.openFile(progress)
        self.writer.openFile()

        while True:
            record = self.reader.readRecord()
            if record == None:
                break
            if len(record) > 0:
                self.writer.writeRecord(record)

        self.reader.closeFile()
        self.writer.closeFile()

Exporter( self )
