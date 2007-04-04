#!/usr/bin/env python

"""
This python script connects KWord and OpenOffice.org together.

The script could be used in two ways;
    1. Embedded in KWord by execution via the "Tools=>Scripts"
       menu or from the "Tools=>Script Manager". In that case
       the document currently loaded and displayed by KWord
       will be used.
    2. As standalone script by running;
            # make the script executable
            chmod 755 `kde4-config --install data`/kword/scripts/extensions/ooimport.py
            # run the script
            `kde4-config --install data`/kword/scripts/extensions/ooimport.py
       In that case the ooimport.py-script will use the with Kross distributed
       krossrunner commandline-application to execute the python script. An empty
       document will be used.

We are using the PyUNO module to access OpenOffice.org. For this an optional hidden
OpenOffice.org instance need to be started. Then the script connects as client to
this OpenOffice.org server instance and controls it.
If the script is run and there connecting to the server failed, then it will (optional)
startup such a OpenOffice.org server instance and shuts it down again once the work is
done. A faster way is, to startup and shutdown the server instance by yourself and then
the script does not need to do it for you each time.
    # Start OpenOffice.org with visible mainwindow
    soffice -nologo "-accept=socket,host=localhost,port=2002;urp;"
    # Start OpenOffice.org in background
    soffice -nologo -norestore -invisible -headless "-accept=socket,host=localhost,port=2002;urp;"

(C)2006 Sebastian Sauer <mail@dipe.org>

http://kross.dipe.org
http://www.koffice.org/kword
http://udk.openoffice.org/python/python-bridge.html

Dual-licensed under LGPL v2+higher and the BSD license.
"""

import sys, os, time, subprocess, signal

try:
    import uno
    from com.sun.star.connection import NoConnectException as UnoNoConnectException
    from com.sun.star.task import ErrorCodeIOException as UnoErrorCodeIOException
    #from com.sun.star import connection as UnoConnection
    from unohelper import Base as UnoBase
    #from unohelper import systemPathToFileUrl, absolutize
    from com.sun.star.beans import PropertyValue as UnoPropertyValue
    #from com.sun.star.uno import Exception as UnoException
    #from com.sun.star.io import IOException as UnoIOException
    from com.sun.star.io import XOutputStream as UnoXOutputStream
except ImportError:
    raise "Failed to import the OpenOffice.org PyUNO python module. The module is required to run this script."

class UnoConfig:
    """ The configuration for to access the OpenOffice.org functionality. """

    def __init__(self):
        # The host the OpenOffice.org Server runs on.
        self.host= "localhost"
        # The port the OpenOffice.org Server runs on.
        self.port = 2002
        # Number of seconds we try to connect before aborting, set to 0 to try to
        # connect only once and -1 to disable timeout and try to connect forever.
        self.connectTimeout = 30
        # Hide the client window.
        self.hideClient = True
        # Close new documents once not needed any longer.
        self.autoCloseDocument = True
        # The used logger we write debug-output to.
        self.logger = sys.stdout

class UnoDocument:
    """ Class that represents an OpenOffice.org UNO document within an UnoClient. """

    class OutputStream( UnoBase, UnoXOutputStream ):
        """ The OutputStream class offers the default implementation of an output-stream
        the content of the document could be written to. """

        def __init__(self):
            self.filterName = "Text (encoded)"
            #self.filterName = "HTML (StarWriter)"
            #self.filterName = "writer_pdf_Export"
            self.closed = 0
        def closeOutput(self):
            self.closed = 1
        def writeBytes(self, seq):
            sys.stdout.write(seq.value)
        def flush(self):
            pass

    def __init__(self, unoConfig, desktop):
        self.unoConfig = unoConfig
        self.desktop = desktop
        self.doc = None

    def __del__(self):
        if self.unoConfig.autoCloseDocument:
            self.close()

    def load(self, fileUrl):
        if not fileUrl.startswith("file://"):
            raise "Invalid file url \"%s\"" % fileUrl

        fileName = fileUrl[7:]
        if not os.path.isfile(fileName):
            raise "There exist no such file \"%s\"" % fileName

        self.close()

        fileBaseName = os.path.basename(fileName)
        self.unoConfig.logger.write("Loading document %s ...\n" % fileBaseName)

        inProps = []
        if self.unoConfig.hideClient:
            inProps.append( UnoPropertyValue("Hidden" , 0 , True, 0) )

        self.doc = self.desktop.loadComponentFromURL(fileUrl , "_blank", 0, tuple(inProps))
        if not self.doc:
            raise "Failed to load document %s" % fileName

        self.unoConfig.logger.write("Done loading document %s\n" % fileBaseName)

    def close(self):
        if self.doc:
            self.doc.close(True)
            self.doc = None

    def read(self, outputstream = OutputStream()):
        outProps = []
        outProps.append( UnoPropertyValue("FilterName" , 0, outputstream.filterName, 0) )
        outProps.append( UnoPropertyValue("Overwrite" , 0, True , 0) )
        outProps.append( UnoPropertyValue("OutputStream", 0, outputstream, 0) )

        try:
            self.doc.storeToURL("private:stream", tuple(outProps))
        except UnoErrorCodeIOException, e:
            self.unoConfig.logger.write("ErrorCodeIOException: %s" % e.ErrCode)

class UnoServer:
    """ Class that provides functionality to deal with the OpenOffice.org server instance. """

    #class ServerThread(threading.Thread):
        #def __init__(self):
            #threading.Thread.__init__(self)
        #def run(self):
            #pass

    def __init__(self, unoConfig):
        self.unoConfig = unoConfig

        self.unoConfig.logger.write("Starting OpenOffice.org at %s:%s ...\n" % (self.unoConfig.host,self.unoConfig.port))
        self.process = subprocess.Popen([
            'soffice',
            '-nologo', #don't show startup screen.
            '-minimized', #keep startup bitmap minimized.
            '-norestore', #suppress restart/restore after fatal errors.
            '-invisible', #no startup screen, no default document and no UI.
            '-headless',
            "-accept=socket,host=%s,port=%s;urp;" % (self.unoConfig.host,self.unoConfig.port)
        ], )

    def __del__(self):
        if hasattr(self,'process'):
            os.kill(self.process.pid, signal.SIGINT)
            #os.kill(self.serverPID, signal.SIGKILL)
            #os.kill(self.serverPID, 9)

class UnoClient:
    """ Class that provides the client-functionality to deal with an OpenOffice.org
    server instance. """

    def __init__(self, unoConfig):
        self.unoConfig = unoConfig
        self.unoServer = None
        self.document = None

        # get the uno component context from the PyUNO runtime
        localContext = uno.getComponentContext()
        # create the UnoUrlResolver
        resolver = localContext.ServiceManager.createInstanceWithContext("com.sun.star.bridge.UnoUrlResolver", localContext)

        # connect to the running office
        elapsed = 0
        while True:
            self.unoConfig.logger.write("Trying to connect with OpenOffice.org on %s:%s ...\n" % (self.unoConfig.host,self.unoConfig.port))
            try:
                # the UNO url we like to resolve
                url = "uno:socket,host=%s,port=%s;urp;StarOffice.ComponentContext" % (self.unoConfig.host,self.unoConfig.port)
                # fetch the ComponentContext
                componentContext = resolver.resolve(url)
                # fetch the service manager
                self.servicemanager = componentContext.ServiceManager
                # create the desktop
                self.desktop = self.servicemanager.createInstanceWithContext("com.sun.star.frame.Desktop", componentContext)
                # create the UnoDocument instance
                self.unoDocument = UnoDocument(self.unoConfig, self.desktop)
                # job is done
                break
            except UnoNoConnectException:
                self.unoConfig.logger.write("Failed to connect with OpenOffice.org on %s:%s ...\n" % (self.unoConfig.host,self.unoConfig.port))
                if not self.unoServer:
                    self.unoServer = UnoServer(self.unoConfig)
                if self.unoConfig.connectTimeout >= 0:
                    if elapsed >= self.unoConfig.connectTimeout:
                        raise "Failed to connect to OpenOffice.org on %s:%s" % (self.unoConfig.host,self.unoConfig.port)
                    elapsed += 1
                time.sleep(1)

        self.unoConfig.logger.write("Connected with OpenOffice.org on %s:%s\n" % (self.unoConfig.host,self.unoConfig.port))

    def __del__(self):
        if self.unoServer:
            self.desktop.terminate()

class UnoController:
    """ Class that offers high level access to control all aspects of OpenOffice.org
    we may need. """

    def __init__(self, unoConfig = UnoConfig()):
        self.unoConfig = unoConfig
        self.unoClient = None

    def connect(self):
        self.unoClient = UnoClient(self.unoConfig)

    def disconnect(self):
        self.unoClient = None

    def loadDocument(self, fileUrl):
        if not self.unoClient:
            raise "The client is not connected"
        self.unoClient.unoDocument.load(fileUrl)

    def writeDocument(self, outputstream = UnoDocument.OutputStream()):
        self.unoClient.unoDocument.read(outputstream)

class KWordOutputStream( UnoDocument.OutputStream ):
    def __init__(self):
            #self.filterName = "Text (encoded)"
            self.filterName = "HTML (StarWriter)"
            #self.filterName = "writer_pdf_Export"

            import KWord
            self.doc = KWord.mainFrameSet().document()
            self.html = ""
    def closeOutput(self):
        #self.doc.setHtml(self.html)
        #self.html = ""
        pass
    def writeBytes(self, seq):
        self.html += seq.value
    def flush(self):
        if self.html != "":
            #print self.html
            self.doc.setHtml(self.html)
            self.html = ""

class ImportDialog:
    def __init__(self, action):
        import Kross
        forms = Kross.module("forms")
        self.dialog = forms.createDialog("Import file with OpenOffice.org")
        self.dialog.setButtons("Ok|Cancel")
        self.dialog.setFaceType("List") #Auto Plain List Tree Tabbed

        openpage = self.dialog.addPage("Open","Import File","fileopen")
        openwidget = forms.createFileWidget(openpage, "kfiledialog:///kwordooimport")
        openwidget.setMode("Opening")
        filters = [
            "*.odt|OpenDocument Text (*.odt)",
            "*.ott|OpenDocument Text Template (*.ott)",
            "*.sxw|OpenOffice.org 1.0 Text Document (*.sxw)",
            "*.stw|OpenOffice.org 1.0 Text Document Template (*.stw)",
            "*.doc|Microsoft Word 97/2000/XP (*.doc)",
            "*.dot|Microsoft Word 97/2000/XP Template (*.dot)",
            "*.doc|Microsoft Word 6.0/95 (*.doc)",
            "*.dot|Microsoft Word 95 Template (*.dot)",
            "*.rtf|Rich Text Format (*.rtf)",
            "*.txt|Text (*.txt)",
            "*.txt|Text Encoded (*.txt)",
            "*.html *.htm|HTML Document (*.html *.htm)",
            "*.xml|DocBook (*.xml)",
            "*.sdw|StarWriter 3.0 - 5.0 (*.sdw)",
            "*.vor|StarWriter 3.0 - 5.0 Templates (*.vor)",
            "*.wpd|WordPerfect Document (*.wpd)",
            "*.lwp|Lotus WordPro Document (*.lwp)",
        ]
        filters.insert(0, "%s|All Supported Files" % " ".join([f.split("|")[0] for f in filters]))
        filters.append("*|All Files")
        openwidget.setFilter("\n".join(filters))

        configpage = self.dialog.addPage("Connect","OpenOffice.org UNO Connection","connect_no")
        configwidget = forms.createWidgetFromUIFile(configpage, os.path.join(action.currentPath(),"ooimportconfig.ui"))

        if self.dialog.exec_loop():
            file = openwidget.selectedFile()
            if not file:
                raise "No file selected"
            if not os.path.isfile(file):
                raise "No such file: %s" % file

            controller = UnoController()

            controller.unoConfig.host = configwidget["hostEdit"].text
            controller.unoConfig.port = configwidget["portEdit"].value
            if configwidget["timeoutCheckBox"].checked:
                controller.unoConfig.connectTimeout = configwidget["timeoutEdit"].text
            else:
                controller.unoConfig.connectTimeout = 0

            controller.connect()
            controller.loadDocument( "file://%s" % file )

            outputstream = KWordOutputStream()
            controller.writeDocument(outputstream)
            outputstream.flush()

            controller.disconnect()

    def __del__(self):
        self.dialog.delayedDestruct()

print "..................1"
dialog = ImportDialog(self)
print "..................2"


#print "CONNECT ..."
#controller.connect()
#print "LOADING DOC ..."
#controller.loadDocument("file:///home/kde4/bla2.odt")
##controller.loadDocument("file:///home/kde4/bla.odt")
#print "DUMP DOC ..."
#controller.writeDocument()
#print "DISCONNECT ..."
#controller.disconnect()
#print "DONE"



#try:
    #opts, args = getopt.getopt(sys.argv[1:], "hc:", ["help", "connection-string=" , "html", "pdf", "stdout" ])
    #url = "uno:socket,host=localhost,port=2002;urp;StarOffice.ComponentContext"
    #ctxLocal = uno.getComponentContext()
    #smgrLocal = ctxLocal.ServiceManager
    #resolver = smgrLocal.createInstanceWithContext("com.sun.star.bridge.UnoUrlResolver", ctxLocal )
    #ctx = resolver.resolve( url )
    #smgr = ctx.ServiceManager
    #desktop = smgr.createInstanceWithContext("com.sun.star.frame.Desktop", ctx )
    #cwd = systemPathToFileUrl( getcwd() )
    #for path in args:
        #try:
            #fileUrl = absolutize( cwd, systemPathToFileUrl(path) )
            #doc = desktop.loadComponentFromURL( fileUrl , "_blank", 0, inProps )
            #if not doc:
                #raise UnoException( "Couldn't open stream for unknown reason", None )
            #if not stdout:
                #(dest, ext) = splitext(path)
                #dest = dest + "." + extension
                #destUrl = absolutize( cwd, systemPathToFileUrl(dest) )
                #sys.stderr.write(destUrl + "\n")
                #doc.storeToURL(destUrl, outProps)
            #else:
                #doc.storeToURL("private:stream",outProps)
        #except IOException, e:
            #sys.stderr.write( "Error during conversion: " + e.Message + "\n" )
        #except UnoException, e:
            #sys.stderr.write( "Error ("+repr(e.__class__)+") during conversion:" + e.Message + "\n" )
        #if doc: doc.dispose()
#except UnoException, e:
    #sys.stderr.write( "Error ("+repr(e.__class__)+") :" + e.Message + "\n" )
#except getopt.GetoptError,e:
    #sys.stderr.write( str(e) + "\n" )
