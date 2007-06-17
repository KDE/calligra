#!/usr/bin/env kross

"""
Python script that provides some functionality to deal with KSpread.scripting.

(C)2006 Sebastian Sauer <mail@dipe.org>
http://kross.dipe.org
http://www.koffice.org/kspread
Dual-licensed under LGPL v2+higher and the BSD license.
"""

import os, datetime, sys, traceback, urlparse

try:
    import Kross
except ImportError:
    raise "Failed to import the Kross scripting module."

class KSpreadReader:
    """ The Reader class provides the functionality to read content from different
    backends like for example a OpenDocument Spreadsheet file by using KSpread. """

    class Config:
        """ Some configurations for the htmlexport.py script. """

        ReaderFiles = ['/home/kde4/kspreaddocument.ods',]
        """ If one of the files exist, use it per default if the htmlexport.py script
        got run from the commandline with the krossrunner application. This option is
        ignored if the htmlexport.py script got executed embedded within KSpreadsince
        in that case always the current document will be used. """

        Infos = {
            'Title' : 'Spreadsheet',
            'Subject' : '',
            'Author' : '',
            'EMail' : '',
            'Keywords' : '',
            'Filename' : '',
        }
        """ The default values for the "HTML Document Informations" page. They are used
        if the corresponding value is not defined (for KSpread see in the mainmenu the
        item "File=>Document Information". """

        Sheets = []
        """ The list of sheet names that should be readed. If the list is empty, what is
        the default, just all sheets are readed. """

    class TestImpl:
        """ The TestImpl class implements a Reader to read static content. This class
        is used mainly for testing purposes. """

        def __init__(self, readerconfig):
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

    class FileImpl:
        """ The FileImpl class implements a Reader that uses KSpread to read
        content from an OpenDocument Spreadsheet file. """

        def __init__(self, readerconfig):
            self.embeddedInKSpread = False
            try:
                import KSpread
                self.kspread = KSpread
                self.embeddedInKSpread = True
            except ImportError:
                try:
                    self.kspread = Kross.module("kspread")
                except ImportError:
                    raise "Failed to import the KSpread scripting module."

            application = self.kspread.application()
            self.document = self.kspread.document()

            self.filename = ''
            for f in readerconfig.ReaderFiles:
                if os.path.isfile(f):
                    self.filename = f
                    break

            if self.embeddedInKSpread:
                if self.document.url():
                    self.setFile(self.document.url())
            elif self.filename and self.filename != '':
                self.setFile(self.filename)
                self.openFile()
            elif self.document.url():
                self.setFile(self.document.url())

            self.infos = {
                'Title' : self.document.documentInfoTitle() or readerconfig.Infos['Title'],
                'Subject' : self.document.documentInfoSubject() or readerconfig.Infos['Subject'],
                'Author' : self.document.documentInfoAuthorName() or readerconfig.Infos['Author'],
                'EMail' : self.document.documentInfoEmail() or readerconfig.Infos['EMail'],
                'Keywords' : self.document.documentInfoKeywords() or readerconfig.Infos['Keywords'],
                'Filename' : self.document.url() or readerconfig.Infos['Filename'],
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
            (scheme, netloc, path, params, query, fragment) = urlparse.urlparse(url)
            return path

        def hasFile(self):
            return self.embeddedInKSpread or (self.filename and self.filename != '')

        def setFile(self, filename):
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

            self.sheet = self.kspread.currentSheet()
            if self.sheet == None:
                sheetnames = self.kspread.sheetNames()
                if len(sheetnames) < 1:
                    raise "No sheets."
                self.sheet = self.kspread.sheetByName( sheetnames[0] )

            self.rowidx = 1
            print "Reader.openFile file=%s rowidx=%i lastRow=%i lastColumn=%i" % (self.filename, self.rowidx, self.sheet.lastRow(), self.sheet.lastColumn())
            self.progress = progress
            if self.progress:
                self.progress.labelText = "Processing sheet \"%s\"" % self.sheet.sheetName()
                self.progress.maximum = self.sheet.lastRow() + 1

        def closeFile(self):
            pass

        def readRecord(self):
            if self.rowidx <= self.sheet.lastRow():
                record = []
                for i in range(self.sheet.lastColumn() + 1, 1, -1):
                    value = self.sheet.text(i, self.rowidx)
                    #print "col=%i row=%i value=%s" % (i, self.rowidx, value)
                    if value or len(record) > 0:
                        record.insert(0,value)
                self.rowidx += 1
                if self.progress:
                    self.progress.value = self.rowidx
                return record
            print "EXPORT DONE rowidx=%i lastRow=%i lastColumn=%i" % (self.rowidx, self.sheet.lastRow(), self.sheet.lastColumn())
            return None

    def __init__(self, readerimpl):
        #readerimpl = Reader.TestImpl(readerconfig)
        #readerimpl = Reader.FileImpl(readerconfig)
        self.impl = readerimpl

    def __getattr__(self, name):
        return getattr(self.impl, name)


