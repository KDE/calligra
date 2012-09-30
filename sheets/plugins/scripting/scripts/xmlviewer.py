#!/usr/bin/env kross

# import some python modules.
import os, sys, traceback, tempfile, zipfile

# import the kross module.
import Kross

T = Kross.module("kdetranslation")

CalligraAppName="Sheets"
CalligraAppExt="ods"

try:
    # try to import the Calligra application. If this fails we are not running embedded.
    CalligraAppModule = __import__(CalligraAppName)
except ImportError:
    # looks as we are not running embedded within the Calligra application. So, let's use Kross to import the library.
    CalligraAppModule = Kross.module(CalligraAppName)

    # testcase that loads a ODT file direct without the probably annoying fileopen-dialog.
    #CalligraAppModule.document().openUrl("/home/kde4/odf/_works/Lists_bulletedList/testDoc/testDoc.odt")

    if not CalligraAppModule.document().url():
        # if the app does not have a loaded document now we show a fileopen-dialog to let the user choose the file.
        forms = Kross.module("forms")
        dialog = forms.createDialog(T.i18n("XML Viewer"))
        dialog.setButtons("Ok|Cancel")
        dialog.setFaceType("Plain")
        openwidget = forms.createFileWidget(dialog.addPage(T.i18n("Open"),T.i18n("Open OpenDocument File")))
        openwidget.setMode("Opening")
        openwidget.setFilter("*.%s|%s\n*|%s" % (CalligraAppExt,T.i18n("OpenDocument Files"),T.i18n("All Files")))
        if not dialog.exec_loop():
            raise Exception(T.i18n("Aborted."))
        CalligraAppModule.document().openUrl(openwidget.selectedFile())

# This class does provide us the viewer dialog we are using to display something to the user.
class Dialog:
    def __init__(self, action):
        # Create the viewer dialog.
        self.forms = Kross.module("forms")
        self.dialog = self.forms.createDialog(T.i18n("XML Viewer"))
        self.dialog.setButtons("Ok")
        self.dialog.setFaceType("List") #Auto Plain List Tree Tabbed
        self.dialog.minimumWidth = 720
        self.dialog.minimumHeight = 500

        # we like to fetch the KoStore which is the backend for the document and does
        # allow us to access files within the store direct.
        doc = CalligraAppModule.document()
        self.store = CalligraAppModule.store()
        self.pages = {}

        # let's read the manifest file by using a KoScriptingOdfReader
        reader = self.store.open("META-INF/manifest.xml")
        if not reader:
            raise Exception(T.i18n("Failed to read the manifest"))
        # walk over all file-entry items the manifest does know about.
        for i in range( reader.count() ):
            # the typename, e.g. "text/xml"
            typeName = reader.type(i)
            # the path/filename, e.g. "content.xml"
            path = reader.path(i)
            widgets = []
            if typeName == "text/xml":
                if not self.store.hasFile(path):
                    print "Skipping not existing path %s" % path
                    continue
                page = self.dialog.addPage(path, "")

                # the browser is used to display some HTML.
                browser = self.forms.createWidget(page, "QTextBrowser", "Editor")
                widgets.append(browser)

                # now some buttons that provide the functionality to open
                # the XML in an external editor like KWrite.
                w = self.forms.createWidget(page, "QWidget")
                self.forms.createLayout(w,"QHBoxLayout")

                openBtn = self.forms.createWidget(w, "QPushButton")
                openBtn.text = "Open with..."
                openBtn.connect("clicked()", self.openClicked)
                widgets.append(openBtn)

                kwriteBtn = self.forms.createWidget(w, "QPushButton")
                kwriteBtn.text = "Open with KWrite"
                kwriteBtn.connect("clicked(bool)", self.kwriteClicked)
                widgets.append(kwriteBtn)

                kxmleditorBtn = self.forms.createWidget(w, "QPushButton")
                kxmleditorBtn.text = "Open with KXMLEditor"
                kxmleditorBtn.connect("clicked(bool)", self.kxmleditorClicked)
                widgets.append(kxmleditorBtn)

                compareBtn = self.forms.createWidget(w, "QPushButton")
                compareBtn.text = "Compare..."
                compareBtn.connect("clicked()", self.compareClicked)
                widgets.append(compareBtn)

                self.pages[path] = [typeName, None, widgets]

        self.currentPageChanged()
        self.dialog.connect("currentPageChanged(KPageWidgetItem*,KPageWidgetItem*)",self.currentPageChanged)
        self.dialog.exec_loop()

    def __del__(self):
        self.dialog.delayedDestruct()

    def currentPageChanged(self, *args):
        # If the current page changed, the user selected another text/xml file-entry item
        # displayed in the dialog as page. We are lazy loading and displaying the XML.
        path = self.dialog.currentPage()
        (text,widgets) = self.pages[path][1:3]
        self._text = text
        if self._text:
            return
        self._text = ""
        self._prevLevel = 0
        # open a KoScriptingOdfReader for the text/xml file.
        reader = self.store.open(path)
        if not reader:
            raise Exception("Failed to open %s" % path)
        # this function will be called for each XML element.
        def onElement():
            # the level the element is on to handle indention of the displayed lists
            level = reader.level()
            if level > self._prevLevel:
                self._text += "<ul>"
                self._prevLevel = level
            if level < self._prevLevel:
                self._text += "</ul>"
                self._prevLevel = level

            # the actual display-name, e.g. "text:p"
            name = reader.name()
            self._text += "<li>%s" % name
            if level > 0:
                # display attributes for all elements except the root-element (which has level==0).
                attributes = [ "%s=%s" % (n,reader.attribute(n)) for n in reader.attributeNames() ]
                if len(attributes) > 0:
                    self._text += " <small>%s</small>" % ", ".join(attributes)
            if not reader.hasChildren() and reader.text():
                # display the text the element may have.
                self._text += "<blockquote>%s</blockquote>" % reader.text()
            self._text += "</li>"
        # connect the KoScriptingOdfReader to our callback function
        reader.connect("onElement()", onElement)
        # start the KoScriptingOdfReader. The reader will then call the callback function for each readed element.
        reader.start()
        # display the result within the browser.
        widgets[0].html = "<style>ul { margin:0; } small { color:#909090; } blockquote { margin:0; color:#0000aa; }</style>%s" % self._text
        self.pages[path][1] = self._text

    def doOpen(self, program):
        # the current page, e.g. "content.xml"
        path = self.dialog.currentPage()
        # the typename, e.g. "text/xml"
        typeName = self.pages[path][0]
        # toFile is the tempfile we like to extract content too.
        toFile = tempfile.mktemp()
        if typeName == "text/xml":
            toFile += ".xml"

        print "doOpen program=\"%s\" path=\"%s\" typeName=\"%s\" toFile=\"%s\"" % (program,path,typeName,toFile)

        # Extract the XML content to a file.
        if not self.store.extractToFile(path,toFile):
            raise Exception(T.i18n("Failed to extract \"%1\" to \"%2\"", [path], [toFile]))
        if not '"' in program:
            program = "\"%s\"" % program

        # Execute the external program with the tempfile as argument.
        result = os.system( "%s \"%s\"" % (program,toFile) )
        if result != 0:
            self.forms.showMessageBox("Error", T.i18n("Error"), T.i18n("<qt>Failed to execute program:<br><br>%1 \"%2\"</qt>", [program], [toFile]))

        # Remove the tempfile again.
        try:
            if os.path.isfile(toFile):
                os.remove(toFile)
        except:
            pass

    def openClicked(self, *args):
        # Show the "Open with..." dialog that allows the user to define the external program
        # that should be used executed.
        dialog = self.forms.createDialog(T.i18n("Open with..."))
        dialog.setButtons("Ok|Cancel")
        dialog.setFaceType("Plain")
        dialog.minimumWidth = 360
        page = dialog.addPage("", "")
        edit = self.forms.createWidget(page, "QLineEdit", T.i18n("Filter"))
        edit.text = 'kate'
        edit.setFocus()
        if dialog.exec_loop():
            program = edit.text.strip()
            dialog.delayedDestruct()
            if not program:
                raise Exception(T.i18n("No program defined."))
            self.doOpen(program)

    def kwriteClicked(self, *args):
        self.doOpen('kwrite')

    def kxmleditorClicked(self, *args):
        self.doOpen('kxmleditor')

    def compareClicked(self, *args):
        # the current page, e.g. "content.xml"
        path = self.dialog.currentPage()
        # the typename, e.g. "text/xml"
        typeName = self.pages[path][0]

        dialog = self.forms.createDialog(T.i18n("Compare..."))
        dialog.setButtons("Ok|Cancel")
        dialog.setFaceType("Plain")
        dialog.minimumWidth = 540
        page = dialog.addPage("", "")

        self.forms.createWidget(page, "QLabel").text = "Command:"
        cmdEdit = self.forms.createWidget(page, "QLineEdit")
        cmdEdit.text = "\"kdiff3\" --L1 \"Current\" --L2 \"OpenDocument File\""

        self._url = CalligraAppModule.document().url()
        self.forms.createWidget(page, "QLabel").text = T.i18n("Compare with OpenDocument file:")
        urlEdit = self.forms.createWidget(page, "KUrlRequester")
        urlEdit.setPath(self._url)
        def editChanged(text):
            self._url = text
        urlEdit.connect("textChanged(QString)", editChanged)
        if not dialog.exec_loop():
            return

        try:
            if self._url.startswith("file://"):
                self._url = self._url[7:]
            if not os.path.isfile(self._url):
                raise Exception(T.i18n("No OpenDocument file to compare with selected."))

            program = cmdEdit.text
            if not program:
                raise Exception(T.i18n("No command selected."))
            if not '"' in program:
                program = "\"%s\"" % program

            # Open the OpenDocument that is just a Zip-file anyway.
            try:
                zf = zipfile.ZipFile(self._url)
            except BadZipfile:
                raise Exception("Invalid OpenDocument file: %s" % self._url)
            # Check if the expected file is there.
            if not path in zf.namelist():
                raise Exception(T.i18n("The OpenDocument file does not contain any file named \"%1\"", [path]))

            # Create the temp-files.
            currentFile = tempfile.mktemp()
            withFile = tempfile.mktemp()
            if typeName == "text/xml":
                currentFile += ".xml"
                withFile += ".xml"

            try:
                # Extract the XML content to a file.
                if not self.store.extractToFile(path,currentFile):
                    raise Exception(T.i18n("Failed to extract \"%1\" to \"%2\"", [path], [currentFile]))

                # Extract the file from the OpenDocument Zip file to the temp-file.
                outfile = open(withFile, 'wb')
                outfile.write(zf.read(path))
                outfile.flush()
                outfile.close()

                # Execute the external program with the tempfile as argument.
                result = os.system( "%s \"%s\" \"%s\"" % (program,currentFile,withFile) )
            finally:
                # Remove the tempfiles again.
                for n in (currentFile,withFile):
                    try:
                        os.remove(n)
                    except:
                        pass
        except:
            self.forms.showMessageBox("Error", T.i18n("Error"), T.i18n("<qt>%1</qt>", [str(sys.exc_value)]))

Dialog(self)
