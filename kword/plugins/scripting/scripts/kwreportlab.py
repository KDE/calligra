#!/usr/bin/env kross

import os, sys, re, types, string, datetime

import reportlab
from reportlab.platypus.doctemplate import PageTemplate, BaseDocTemplate
from reportlab.lib.units import inch, cm
from reportlab.lib.pagesizes import A4
from reportlab.lib.styles import getSampleStyleSheet, ParagraphStyle
from reportlab.platypus.paragraph import Paragraph
from reportlab.platypus.flowables import PageBreak
from reportlab.platypus.frames import Frame
#from string import find, join, split, replace, expandtabs, rstrip
#from reportlab.pdfgen import canvas
#from reportlab.lib import colors, enums
#from reportlab.lib.enums import TA_CENTER, TA_LEFT
#from reportlab.platypus.flowables import Flowable, Preformatted, Spacer, Image, KeepTogether
#from reportlab.platypus.tableofcontents import TableOfContents
#from reportlab.platypus.xpreformatted import XPreformatted
#from reportlab.platypus.doctemplate import PageTemplate, BaseDocTemplate
#from reportlab.platypus.tables import TableStyle, Table

import Kross

try:
    import KWord
except ImportError:
    KWord = Kross.module("kword")

class MyConfig:
    def __init__(self):
        self.showDialog = True
        #self.showDialog = False

        self.readOdfFile = ""
        #self.readOdfFile = "/home/kde4/odf/_works/Lists_bulletedList/testDoc/testDoc.odt"

        self.writeFileName = ""
        #self.writeFileName = "/home/kde4/__MyReportLabTest.pdf"

class MyWriter:

    class MyTemplate(BaseDocTemplate):
        _invalidInitArgs = ('pageTemplates',)

        def __init__(self, writer):
            self.writer = writer
            self.config = writer.config
            self.kwdoc = writer.kwdoc
            apply(BaseDocTemplate.__init__, (self, self.config.writeFileName), )
            x = 2.5*cm
            y = 2.5*cm
            width = 15*cm
            height = 25*cm
            mainFrame = Frame(x, y, width, height, id='MainFrame')
            page = PageTemplate('normal', [mainFrame,], self.onPage, self.onPageEnd)
            self.addPageTemplates(page)

        def onPage(self, canvas, doc):
            print "=> onPage"
            canvas.saveState()

            # header
            title = self.kwdoc.documentInfoTitle()
            if not title:
                title = self.kwdoc.documentInfoSubject()
            if title:
                canvas.setFont('Times-Roman', 12)
                canvas.drawString(2*cm, A4[1]-1.2*cm, title)

            author = self.kwdoc.documentInfoAuthorName()
            company = self.kwdoc.documentInfoCompanyName()
            mail = self.kwdoc.documentInfoEmail()
            if company:
                if author:
                    author = "%s, %s" % (company,author)
                else:
                    author = company
            if mail:
                if author:
                    author = "%s, %s" % (author,mail)
                else:
                    author = mail
            if author:
                canvas.setFont('Times-Roman', 10)
                canvas.drawString(2*cm, A4[1]-1.2*cm-14, author)

            canvas.line(2*cm, A4[1]-2*cm, A4[0]-2*cm, A4[1]-2*cm)

            # body
            #self.reader.drawPageBody(canvas)

            # footer
            canvas.line(2*cm, 2*cm, A4[0]-2*cm, 2*cm)
            #if hasattr(canvas, 'headerLine'): # hackish
            #    headerline = string.join(canvas.headerLine, ' \215 ') # bullet
            #    canvas.drawString(2*cm, A4[1]-1.75*cm, headerline)
            canvas.setFont('Times-Roman', 8)
            canvas.drawString(2*cm, 1.65*cm, self.config.writeFileName)
            canvas.drawString(2*cm, 1.65*cm - 10, datetime.datetime.now().strftime("%Y-%m-%d %H:%M"))
            canvas.setFont('Times-Roman', 12)
            pageNumber = canvas.getPageNumber()
            canvas.drawString(4 * inch, cm, "%d" % pageNumber)

            canvas.restoreState()

        def onPageEnd(self, canvas, doc):
            print "=> onPageEnd"

        def afterFlowable(self, flowable):
            print "=> afterFlowable %s" % flowable.__class__.__name__
            if flowable.__class__.__name__ == 'Paragraph':
                f = flowable
                #name7 = f.style.name[:7]
                #name8 = f.style.name[:8]
                print "   style.name=%s" % f.style.name

                ## Build a list of heading parts.
                ## So far, this is the *last* item on the *previous* page...
                #if name7 == 'Heading' and not hasattr(self.canv, 'headerLine'):
                    #self.canv.headerLine = []
                #if name8 == 'Heading0':
                    #self.canv.headerLine = [f.text] # hackish
                #elif name8 == 'Heading1':
                    #if len(self.canv.headerLine) == 2:
                        #del self.canv.headerLine[-1]
                    #elif len(self.canv.headerLine) == 3:
                        #del self.canv.headerLine[-1]
                        #del self.canv.headerLine[-1]
                    #self.canv.headerLine.append(f.text)
                #elif name8 == 'Heading2':
                    #if len(self.canv.headerLine) == 3:
                        #del self.canv.headerLine[-1]
                    #self.canv.headerLine.append(f.text)
                #if name7 == 'Heading':
                    ## Register TOC entries.
                    #headLevel = int(f.style.name[7:])
                    #self.notify('TOCEntry', (headLevel, flowable.getPlainText(), self.page))
                    ## Add PDF outline entries.
                    #c = self.canv
                    #title = f.text
                    #key = str(hash(f))
                    #try:
                        #if headLevel == 0: isClosed = 0
                        #else: isClosed = 1
                        #c.bookmarkPage(key)
                        #c.addOutlineEntry(title, key, level=headLevel, closed=isClosed)
                    #except ValueError:
                        #pass

    def __init__(self, config):
        self.config = config
        self.kwdoc = KWord.document()
        self.doc = MyWriter.MyTemplate(self)
        self.style = getSampleStyleSheet()

    def getParagraphStyle(self, styleName):
        try:
            return self.style[styleName]
        except KeyError:
            parentStyle = None
            style = ParagraphStyle(styleName, parentStyle)

            #style.defaults['fontName'] = 'Times-Roman'
            #style.defaults['fontSize'] = 20
            #style.defaults['fontSize'] = 20
            #style.defaults['leading'] = 12
            #style.defaults['leftIndent'] = 0
            #style.defaults['rightIndent'] = 0
            #style.defaults['firstLineIndent'] = 0
            #style.defaults['alignment'] = TA_LEFT
            #style.defaults['spaceBefore'] = 0
            #style.defaults['spaceAfter'] = 0
            #style.defaults['bulletFontName'] = 'Times-Roman'
            #style.defaults['bulletFontSize'] = 10
            #style.defaults['bulletIndent'] = 0
            #style.defaults['textColor'] = black

            self.style.add(style)
            return style

    def write(self):
        style = getSampleStyleSheet()
        story = []
        #story.append(Paragraph('<font size=18>Generated by: docpy.py version %s</font>' %  __version__, self.bt))
        #story.append(Paragraph('<font size=18>Date generated: %s</font>' % timeString, self.bt))

        kwdoc = KWord.document()
        if self.config.readOdfFile:
            kwdoc.openUrl(self.config.readOdfFile)

        # Get the KoStore backend for the file.
        store = KWord.store()

        # We like to read the content.xml file from the KoStore.
        reader = store.readFile("content.xml")
        if not reader:
            raise "Failed to read file from the store"

        # This method got called on each readed element.
        def onElement():
            if reader.isNull():
                return
            if reader.name() == "text:p":
                print "onElement: text:p name=%s namespaceURI=%s level=%s" % (reader.name(),reader.namespaceURI(),reader.level())
                text = reader.text()
                if not text:
                    return
                styleName = reader.attribute("text:style-name","default")
                style = self.getParagraphStyle(styleName) #styleName='BodyText'
                story.append( Paragraph("%s" % text, style) )
            elif reader.name() == "style:style":
                print "onElement: style:style attributeNames=%s styleName=%s" % (reader.attributeNames(),reader.attribute('style:name'))
            else:
                print "onElement: Unhandled name=%s" % reader.name()
            #print "  attributeNames=%s" % reader.attributeNames()
            #print "  isElement=%s isText=%s" % (reader.isElement(),reader.isText())

        # Connect the onElement-signal with our function above.
        reader.connect("onElement()", onElement)

        # Start the reading.
        reader.start()

        #story.append( Paragraph('<font size=18>Some text !!!</font>', style['BodyText']) )
        #story.append( PageBreak() )
        #story.append( Paragraph('<font size=18>Some</font>', style['BodyText']))
        #story.append( PageBreak() )
        self.doc.multiBuild(story)

class MyDialog:
    def __init__(self, action, config):
        self.config = config

        forms = Kross.module("forms")
        self.dialog = forms.createDialog("ReportLab")
        self.dialog.setButtons("Ok|Cancel")
        self.dialog.setFaceType("List") #Auto Plain List Tree Tabbed
        self.dialog.minimumWidth = 580
        self.dialog.minimumHeight = 400

        #if not self.config.writeFileName:
        savepage = self.dialog.addPage("Save","Export to PDF Document","document-save")
        savewidget = forms.createFileWidget(savepage, "kfiledialog:///kwordreportlab")
        savewidget.setFilter("*.pdf|PDF Documents\n*|All Files")
        savewidget.setMode("Saving")

        #layoutpage = self.dialog.addPage("Page","Page Options","book")
        #layoutwidget = forms.createWidgetFromUIFile(layoutpage, os.path.join(action.currentPath(), "kwreportlabpage.ui"))

        if self.dialog.exec_loop():
            if not self.config.writeFileName:
                self.config.writeFileName = savewidget.selectedFile()
                if not self.config.writeFileName:
                    raise "No file selected."
            writer = MyWriter(config)
            writer.write()

    def __del__(self):
        self.dialog.delayedDestruct()

config = MyConfig()

if not config.writeFileName:
    config.showDialog = True

if config.showDialog:
    MyDialog(self, config)
else:
    writer = MyWriter(config)
    writer.write()
