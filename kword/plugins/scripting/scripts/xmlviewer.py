#!/usr/bin/env kross

# import some python modules.
import os, sys, tempfile

# import the kross module.
import Kross

try:
    # try to import KWord. If this fails we are not running embedded in KWord.
    import KWord
except ImportError:
    # looks as we are not running embedded within KWord. So, let's use Kross to import the KWord library.
    KWord = Kross.module("kword")

    # testcase that loads a ODT file direct without the probably annoying fileopen-dialog.
    #KWord.document().openUrl("/home/kde4/odf/_works/Lists_bulletedList/testDoc/testDoc.odt")

    if not KWord.document().url():
        # if KWord does not have a loaded document now we show a fileopen-dialog to let the user choose the odt file.
        forms = Kross.module("forms")
        dialog = forms.createDialog("XML Viewer")
        dialog.setButtons("Ok|Cancel")
        dialog.setFaceType("Plain")
        openwidget = forms.createFileWidget(dialog.addPage("Open","Open ODT File"))
        openwidget.setMode("Opening")
        openwidget.setFilter("*.odt|ODT Files\n*|All Files")
        if not dialog.exec_loop():
            raise "Aborted."
        KWord.document().openUrl(openwidget.selectedFile())

# This class does provide us the viewer dialog we are using to display something to the user.
class Dialog:
    def __init__(self, action):
        # Create the viewer dialog.
        self.forms = Kross.module("forms")
        self.dialog = self.forms.createDialog("XML Viewer")
        self.dialog.setButtons("Ok")
        self.dialog.setFaceType("List") #Auto Plain List Tree Tabbed
        self.dialog.minimumWidth = 720
        self.dialog.minimumHeight = 500

        # we like to fetch the KoStore which is the backend for the document and does
        # allow us to access files within the store direct.
        doc = KWord.document()
        self.store = KWord.store()
        self.pages = {}

        # let's read the manifest file by using a KoScriptingOdfReader
        reader = self.store.open("META-INF/manifest.xml")
        if not reader:
            raise "Failed to read the mainfest"
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
            raise "failed to open %s" % path
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
            #print "isElement=%s isText=%s hasChildren=%s text=%s" % (reader.isElement(),reader.isText(),reader.hasChildren(),reader.text())
            if reader.name().startswith("text:") and reader.text():
                # display the text the element may have.
                self._text += "<blockquote>%s</blockquote>" % reader.text()
            self._text += "</li>"
        # connect the KoScriptingOdfReader to our callback function
        reader.connect("onElement()", onElement)
        # start the KoScriptingOdfReader. The reader will then call the callback function for each readed element.
        reader.start()
        # display the result within the browser.
        widgets[0].html = "<style>ul { margin:0; } small { color:#909090; } blockquote { margin:0; color:#000099; }</style>%s" % self._text
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
            raise "Failed to extract \"%s\" to \"%s\"" % (path,toFile)
        if not ( program.startswith('"') and program.endswith('"') ):
            program = "\"%s\"" % program

        # Execute the external program with the tempfile as argument.
        result = os.system( "%s \"%s\"" % (program,toFile) )
        if result != 0:
            self.forms.showMessageBox("Error", "Error", "<qt>Failed to execute program:<br><br>%s \"%s\"</qt>" % (program,toFile))

        # Remove the tempfile again.
        try:
            if os.path.isfile(toFile):
                os.remove(toFile)
        except:
            pass

    def openClicked(self, *args):
        # Show the "Open with..." dialog that allows the user to define the external program
        # that should be used executed.
        dialog = self.forms.createDialog("Open with...")
        dialog.setButtons("Ok|Cancel")
        dialog.setFaceType("Plain") #Auto Plain List Tree Tabbed
        dialog.minimumWidth = 360
        page = dialog.addPage("", "")
        edit = self.forms.createWidget(page, "QLineEdit", "Filter")
        edit.text = 'kate'
        edit.setFocus()
        if dialog.exec_loop():
            program = edit.text.strip()
            dialog.delayedDestruct()
            if not program:
                raise "No program defined."
            self.doOpen(program)

    def kwriteClicked(self, *args):
        self.doOpen('kwrite')

    def kxmleditorClicked(self, *args):
        self.doOpen('kxmleditor')

Dialog(self)
