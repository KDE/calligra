#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
This python script connects with OpenOffice.org using PyUNO and provides
us the functionality to control OpenOffice.org.

    # Execute the python-script with the ODT-file as argument
    python `kde4-config --install data`/words/scripts/extensions/oouno.py /path/mydoc.odt
    # Define the hostaddress and port the OpenOffice.org server is running on
    python `kde4-config --install data`/words/scripts/extensions/oouno.py --host=192.168.0.1 --port=2002 /path/mydoc.odt

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

(C)2007 Sebastian Sauer <mail@dipe.org>

http://kross.dipe.org
http://www.calligra.org/words
http://udk.openoffice.org/python/python-bridge.html

Dual-licensed under LGPL v2+higher and the BSD license.
"""

import sys, os, getopt, time, traceback, popen2, subprocess, signal #, threading

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
except ImportError, e:
    print >> sys.stderr, "Failed to import the OpenOffice.org PyUNO python module. This script requires the PyUNO python module to communicate with the OpenOffice.org server."
    raise e

class UnoConfig:
    """ The configuration for to access the OpenOffice.org functionality. """

    def __init__(self):
        # The host the OpenOffice.org Server runs on.
        self.host= "localhost"
        # The port the OpenOffice.org Server runs on.
        self.port = 2002
        # Number of seconds we try to connect before aborting, set to 0 to try to
        # connect only once and -1 to disable timeout and try to connect forever.
        self.timeout = 45
        # Startup OpenOffice.org instance if not running already.
        self.startupServer = True
        # Hide the client window.
        self.hideClient = True
        # Close new documents once not needed any longer.
        self.autoCloseDocument = True
        # The used logger we write debug-output to.
        self._logger = sys.stdout

        # The file to load.
        self.loadfile = ""
        # The file to save.
        self.savefile = ""

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
        self.unoConfig._logger.write("Loading document %s ...\n" % fileBaseName)

        inProps = []
        if self.unoConfig.hideClient:
            inProps.append( UnoPropertyValue("Hidden" , 0 , True, 0) )

        self.doc = self.desktop.loadComponentFromURL(fileUrl , "_blank", 0, tuple(inProps))
        if not self.doc:
            raise "Failed to load document %s" % fileName

        self.unoConfig._logger.write("Done loading document %s\n" % fileBaseName)

    def save(self, fileUrl):
        if not self.doc:
            raise "Failed to save cause there is no document"
        if fileUrl.startswith("file://"):
            fileUrl = fileUrl[7:]
        if not fileUrl:
            raise "Failed to save cause invalid file \"%s\" defined." % fileUrl

        try:
            import unohelper
            outUrl = unohelper.systemPathToFileUrl(fileUrl)
            outProps = []

            fileExt = os.path.splitext(fileUrl)[1].lower()
            if fileExt == '.txt' or fileExt == '.text':
                outProps.append( UnoPropertyValue('FilterName', 0, 'Text (encoded)', 0) )
            elif fileExt == '.htm' or fileExt == '.html':
                outProps.append( UnoPropertyValue('FilterName', 0, 'HTML (StarWriter)', 0) )
            elif fileExt == '.pdf':
                outProps.append( UnoPropertyValue('FilterName', 0, 'writer_pdf_Export', 0) )
            #else: opendocument...

            print "Save to: %s" % outUrl
            self.doc.storeToURL(outUrl, tuple(outProps))
        except:
            traceback.print_exc()

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
            self.unoConfig._logger.write("ErrorCodeIOException: %s" % e.ErrCode)

class UnoServer:
    """ Class that provides functionality to deal with the OpenOffice.org server instance. """

    def __init__(self, unoConfig):
        self.unoConfig = unoConfig

        self.unoConfig._logger.write("Starting OpenOffice.org at %s:%s ...\n" % (self.unoConfig.host,self.unoConfig.port))
        try:
            self.process = popen2.Popen3([ #subprocess.Popen([
                'soffice',
                '-nologo', #don't show startup screen.
                '-minimized', #keep startup bitmap minimized.
                '-norestore', #suppress restart/restore after fatal errors.
                '-invisible', #no startup screen, no default document and no UI.
                '-headless',
                "-accept=socket,host=%s,port=%s;urp;" % (self.unoConfig.host,self.unoConfig.port)
            ], )
        except IOError:
            traceback.print_exc()
            raise

    def __del__(self):
        if hasattr(self,'process'):
            os.kill(self.process, signal.SIGKILL)
            #os.kill(self.process.pid, signal.SIGINT)
            #killedpid, stat = os.waitpid(self.process, os.WNOHANG)
            #if killedpid == 0:
                #print >> sys.stderr, "Failed to kill OpenOffice.org Server process" 

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
            self.unoConfig._logger.write("Trying to connect with OpenOffice.org on %s:%s ...\n" % (self.unoConfig.host,self.unoConfig.port))
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
                self.unoConfig._logger.write("Failed to connect with OpenOffice.org on %s:%s ...\n" % (self.unoConfig.host,self.unoConfig.port))
                if self.unoConfig.startupServer:
                    if not self.unoServer:
                        self.unoServer = UnoServer(self.unoConfig)
                if self.unoConfig.timeout >= 0:
                    if elapsed >= self.unoConfig.timeout:
                        raise "Failed to connect to OpenOffice.org on %s:%s" % (self.unoConfig.host,self.unoConfig.port)
                    elapsed += 1
                time.sleep(1)

        self.unoConfig._logger.write("Connected with OpenOffice.org on %s:%s\n" % (self.unoConfig.host,self.unoConfig.port))

    def __del__(self):
        if self.unoServer:
            self.desktop.terminate()
            time.sleep(1)
            self.unoServer = None

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

    def saveDocument(self, fileUrl):
        if not self.unoClient:
            raise "The client is not connected"
        self.unoClient.unoDocument.save(fileUrl)

    def writeDocument(self, outputstream = UnoDocument.OutputStream()):
        self.unoClient.unoDocument.read(outputstream)

#class WordsOutputStream( UnoDocument.OutputStream ):
    #def __init__(self, unoConfig):
            ##self.filterName = "Text (encoded)"
            #self.filterName = "HTML (StarWriter)"
            ##self.filterName = "writer_pdf_Export"

            #import Words
            #self.doc = Words.mainFrameSet().document()
            #self.html = ""

    #def closeOutput(self):
        ##self.doc.setHtml(self.html)
        ##self.html = ""
        #pass
    #def writeBytes(self, seq):
        #self.html += seq.value
    #def flush(self):
        #if self.html != "":
            ##print self.html
            #self.doc.setHtml(self.html)
            #self.html = ""

def start(unoconfig, opts, args):
    print "ARGS: ", "".join(args)
    print "OPTS: ", "\n".join( [ "%s=%s" % (s,getattr(unoconfig,s)) for s in dir(unoconfig) if not s.startswith('_') ] )

    #class ProgressThread(threading.Thread):
        #def __init__(self, unoconfig):
            #self.done = False
            #self.unoconfig = unoconfig
            #threading.Thread.__init__(self)
            ##self.progress = self.forms.showProgressDialog("Import...", "Initialize...")
            ##self.progress.labelText = "Loading %s" % file
            ##self.progress.value = 0
            ##self.progress.update()
        #def finish(self):
            #if not self.done:
                ##self.progress.value = 100
                #self.done = True
        #def run(self):
            #while not self.done:
                ##if self.value == self.progress.value:
                ##    self.value = self.value + 1
                ##self.progress.value = self.value
                #time.sleep(1)
            ##self.progress.reset()
    #progressThread = ProgressThread(unoconfig)
    #progressThread.start()
    #progressThread.finish()
    #progressThread.join() # wait till the thread finished

    controller = UnoController(unoconfig)
    controller.connect()
    try:
        if unoconfig.loadfile:
            controller.loadDocument( "file://%s" % unoconfig.loadfile )

        if unoconfig.savefile:
            controller.saveDocument( "file://%s" % unoconfig.savefile )

        #TODO disabled for now
        #outputstream = UnoDocument.OutputStream()
        #controller.writeDocument(outputstream)
        #outputstream.flush()

    finally:
        controller.disconnect()

def main(argv):
    unoconfig = UnoConfig()

    def usage():
        print "Syntax:\n  %s [options]" % os.path.split(argv[0])[1]
        print "\n  ".join([ "Options:", "--help prints usage informations", ])
        for s in dir(unoconfig):
            if not s.startswith('_'):
                v = getattr(unoconfig,s)
                print "  --%s (%s, %s)" % (s,type(v).__name__,v)
    try:
        opts, args = getopt.getopt(argv[1:], "h", ["help"] + [ "%s=" % s for s in dir(unoconfig) if not s.startswith('_') ])
    except getopt.GetoptError, e:
        usage()
        print "\nArgument Error: ",e,"\n"
        sys.exit(2)

    for opt, arg in opts:
        if opt in ("-h", "--help"):
            usage()
            sys.exit()
        elif opt.startswith('--'):
            n = opt[2:]
            if not n.startswith('_'):
                try:
                    t = type( getattr(unoconfig,n) )
                    if t == bool:
                        setattr(unoconfig, n, arg and arg != 'None' and arg != 'False' and arg != '0')
                    else:
                        setattr(unoconfig, n, t(arg))
                except ValueError, e:
                    print "Argument Error: ",e,"\n"
                    usage()
                    sys.exit(2)

    start(unoconfig, opts, args)

if __name__ == "__main__":
    main(sys.argv)
