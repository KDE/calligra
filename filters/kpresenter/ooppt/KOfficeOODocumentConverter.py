#
# PyODConverter (Python OpenDocument Converter) v1.0.0 - 2008-05-05
#
# This script converts a document from one office format to another by
# connecting to an OpenOffice.org instance via Python-UNO bridge.
#
# Copyright (C) 2008 Mirko Nasato <mirko@artofsolving.com>
# Licensed under the GNU LGPL v2.1 - http://www.gnu.org/licenses/lgpl-2.1.html
# - or any later version.
#
# Copyright (C) 2007 Sebastian Sauer <mail@dipe.org>
# Copyright (C) 2009 Sven Langkamp <sven.langkamp@gmail.com>

DEFAULT_OPENOFFICE_PORT = 8100

import sys, os, getopt, time, traceback, subprocess, signal

# this was needed to find uno
sys.path.append("/usr/lib/ooo-2.0/program") 

import uno
from os.path import abspath, isfile, splitext
from com.sun.star.beans import PropertyValue
from com.sun.star.task import ErrorCodeIOException
from com.sun.star.connection import NoConnectException

class UnoServer:
    """ Class that provides functionality to deal with the OpenOffice.org server instance. """

    def __init__(self, port=DEFAULT_OPENOFFICE_PORT):
        #self.unoConfig._logger.write("Starting OpenOffice.org at %s:%s ...\n" % (self.unoConfig.host,self.unoConfig.port))
        try:
            self.process = subprocess.Popen([
                'soffice',
                '-nologo', #don't show startup screen.
                '-minimized', #keep startup bitmap minimized.
                '-norestore', #suppress restart/restore after fatal errors.
                '-invisible', #no startup screen, no default document and no UI.
                '-headless',
                "-accept=socket,host=localhost,port=%s;urp;" % port
            ], )
        except IOError:
            traceback.print_exc()
            raise

    def __del__(self):
        if hasattr(self,'process'):
            os.kill(self.process.pid, signal.SIGKILL)

FAMILY_TEXT = "Text"
FAMILY_SPREADSHEET = "Spreadsheet"
FAMILY_PRESENTATION = "Presentation"
FAMILY_DRAWING = "Drawing"

FILTER_MAP = {
    "pdf": {
        FAMILY_TEXT: "writer_pdf_Export",
        FAMILY_SPREADSHEET: "calc_pdf_Export",
        FAMILY_PRESENTATION: "impress_pdf_Export",
        FAMILY_DRAWING: "draw_pdf_Export"
    },
    "html": {
        FAMILY_TEXT: "HTML (StarWriter)",
        FAMILY_SPREADSHEET: "HTML (StarCalc)",
        FAMILY_PRESENTATION: "impress_html_Export"
    },
    "odt": { FAMILY_TEXT: "writer8" },
    "doc": { FAMILY_TEXT: "MS Word 97" },
    "rtf": { FAMILY_TEXT: "Rich Text Format" },
    "txt": { FAMILY_TEXT: "Text" },
    "ods": { FAMILY_SPREADSHEET: "calc8" },
    "xls": { FAMILY_SPREADSHEET: "MS Excel 97" },
    "odp": { FAMILY_PRESENTATION: "impress8" },
    "ppt": { FAMILY_PRESENTATION: "MS PowerPoint 97" },
    "swf": { FAMILY_PRESENTATION: "impress_flash_Export" }
}
# see http://wiki.services.openoffice.org/wiki/Framework/Article/Filter
# for more available filters


class DocumentConversionException(Exception):

    def __init__(self, message):
        self.message = message

    def __str__(self):
        return self.message


class DocumentConverter:
    
    def __init__(self, port=DEFAULT_OPENOFFICE_PORT):
        localContext = uno.getComponentContext()
        resolver = localContext.ServiceManager.createInstanceWithContext("com.sun.star.bridge.UnoUrlResolver", localContext)
        # try 10 seconds to connect
        tryStartTime = time.time()
        context = None
        while tryStartTime + 10 > time.time() and context == None:
            try:
                context = resolver.resolve("uno:socket,host=localhost,port=%s;urp;StarOffice.ComponentContext" % port)
            except NoConnectException:
                context = None
        if context == None:
            raise DocumentConversionException, "failed to connect to OpenOffice.org on port %s" % port
        self.desktop = context.ServiceManager.createInstanceWithContext("com.sun.star.frame.Desktop", context)

    def convert(self, inputFile, outputFile):

        inputUrl = self._toFileUrl(inputFile)
        outputUrl = self._toFileUrl(outputFile)
        
        document = self.desktop.loadComponentFromURL(inputUrl, "_blank", 0, self._toProperties(Hidden=True))
        try:
          document.refresh()
        except AttributeError:
          pass
        
        outputExt = self._getFileExt(outputFile)
        filterName = self._filterName(document, outputExt)

        try:
            document.storeToURL(outputUrl, self._toProperties(FilterName=filterName))
        finally:
            document.close(True)

    def _filterName(self, document, outputExt):
        family = self._detectFamily(document)
        try:
            filterByFamily = FILTER_MAP[outputExt]
        except KeyError:
            raise DocumentConversionException, "unknown output format: '%s'" % outputExt
        try:
            return filterByFamily[family]
        except KeyError:
            raise DocumentConversionException, "unsupported conversion: from '%s' to '%s'" % (family, outputExt)
    
    def _detectFamily(self, document):
        if document.supportsService("com.sun.star.text.GenericTextDocument"):
            # NOTE: a GenericTextDocument is either a TextDocument, a WebDocument, or a GlobalDocument
            # but this further distinction doesn't seem to matter for conversions
            return FAMILY_TEXT
        if document.supportsService("com.sun.star.sheet.SpreadsheetDocument"):
            return FAMILY_SPREADSHEET
        if document.supportsService("com.sun.star.presentation.PresentationDocument"):
            return FAMILY_PRESENTATION
        if document.supportsService("com.sun.star.drawing.DrawingDocument"):
            return FAMILY_DRAWING
        raise DocumentConversionException, "unknown document family: %s" % document

    def _getFileExt(self, path):
        ext = splitext(path)[1]
        if ext is not None:
            return ext[1:].lower()

    def _toFileUrl(self, path):
        return uno.systemPathToFileUrl(abspath(path))

    def _toProperties(self, **args):
        props = []
        for key in args:
	    prop = PropertyValue()
	    prop.Name = key
	    prop.Value = args[key]
	    props.append(prop)
        return tuple(props)


if __name__ == "__main__":
    from sys import argv, exit
    
    if len(argv) < 3:
        print "USAGE: python %s <input-file> <output-file>" % argv[0]
        exit(255)
    if not isfile(argv[1]):
        print "no such input file: %s" % argv[1]
        exit(1)

    print  argv[1] + " " + argv[2]

    server = UnoServer()

    try:
        outputFilename = argv[2] + ".odp"
        converter = DocumentConverter()    
        converter.convert(argv[1], outputFilename)
        os.system("mv " + outputFilename + " " + argv[2])
    except DocumentConversionException, exception:
        print "ERROR!" + str(exception)
        exit(1)
    except ErrorCodeIOException, exception:
        print "ERROR! ErrorCodeIOException %d" % exception.ErrCode
        exit(1)

