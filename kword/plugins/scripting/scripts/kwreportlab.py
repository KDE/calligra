#!/usr/bin/env kross

import os, sys, re, types, string, datetime, urllib

import reportlab
from reportlab.platypus.doctemplate import PageTemplate, BaseDocTemplate
from reportlab.lib.units import inch, cm
from reportlab.lib.pagesizes import A0,A1,A2,A3,A4,A5,A6,B0,B1,B2,B3,B4,B5,B6,letter
from reportlab.lib.styles import getSampleStyleSheet, ParagraphStyle
from reportlab.platypus.paragraph import Paragraph
from reportlab.platypus.flowables import PageBreak, Spacer
from reportlab.platypus.frames import Frame
#from reportlab.pdfgen import canvas
#from reportlab.lib import colors, enums
from reportlab.lib.enums import TA_LEFT,TA_CENTER,TA_RIGHT,TA_JUSTIFY
#from reportlab.platypus.flowables import Flowable, Preformatted, Image, KeepTogether
#from reportlab.platypus.tableofcontents import TableOfContents
#from reportlab.platypus.xpreformatted import XPreformatted
#from reportlab.platypus.tables import TableStyle, Table

import Kross
try:
    import KWord
    embeddedInKWord = True
except ImportError:
    KWord = Kross.module("kword")
    embeddedInKWord = False

class MyConfig:

    def __init__(self):
        #self.showDialog = True
        self.showDialog = False

        #self.readOdfFile = ""
        #self.readOdfFile = "/home/kde4/odf/_works/Lists_bulletedList/testDoc/testDoc.odt"
        #self.readOdfFile = "/home/kde4/odf/_works/textFormatting_alignment/testDoc/testDoc.odt"
        self.readOdfFile = "/home/kde4/odf/_works/Paragraph_AttributedText/testDoc/bold.odt"
        #self.readOdfFile = "/home/kde4/odf/_works/textFormatting_fontSize/testDoc/testDoc.odt"

        #self.writeFileName = ""
        self.writeFileName = "/home/kde4/__MyReportLabTest.pdf"

        self.pageSize = A4 # [width,height]
        #self.pageMargin = [ 2.0, 2.0, 2.0, 2.0 ] # [left,top,width,height]
        self.pageCompression = 1 # 0=No compression, 1=Enable compression

class MyWriter:

    class MyTemplate(BaseDocTemplate):
        _invalidInitArgs = ('pageTemplates',)

        def __init__(self, writer):
            self.writer = writer
            self.config = writer.config
            self.kwdoc = writer.kwdoc

            apply(BaseDocTemplate.__init__, (self, self.config.writeFileName), )

            (x,y,width,height) = (2.0*cm, 2.0*cm, 15.0*cm, 25.0*cm)
            mainFrame = Frame(x, y, width, height, id='MainFrame')
            page = PageTemplate('normal', [mainFrame,], self.onPage, self.onPageEnd)
            self.addPageTemplates(page)

        def onPage(self, canvas, doc):
            print "onPage PageNumber=%s" % canvas.getPageNumber()

            if not hasattr(self,"_title"):
                self._title = self.kwdoc.documentInfoTitle()
                if not self._title:
                    self._title = self.kwdoc.documentInfoSubject()
                    if not self._title:
                        self._title = self.kwdoc.url()

            if not hasattr(self,"_author"):
                self._author = self.kwdoc.documentInfoAuthorName()
                company = self.kwdoc.documentInfoCompanyName()
                mail = self.kwdoc.documentInfoEmail()
                if company:
                    if self._author:
                        self._author = "%s, %s" % (company, self._author)
                    else:
                        self._author = company
                if mail:
                    if self._author:
                        self._author = "%s, %s" % (self._author, mail)
                    else:
                        self._author = mail

            canvas.saveState()

            #canvas.setAuthor()
            #canvas.addOutlineEntry(title, key, level=0, closed=None)
            #canvas.setTitle(title)
            #canvas.setSubject(subj)
            #canvas.pageHasData()
            #canvas.showOutline()
            #canvas.bookmarkPage(name)
            #canvas.bookmarkHorizontalAbsolute(name, yhorizontal)
            #canvas.doForm()
            #canvas.beginForm(name, lowerx=0, lowery=0, upperx=None, uppery=None)
            #canvas.endForm()
            #canvas.linkAbsolute(contents, destinationname, Rect=None, addtopage=1, name=None, **kw)
            #canvas.linkRect(contents, destinationname, Rect=None, addtopage=1, relative=1, name=None, **kw)
            #canvas.addLiteral()
            #canvas.stringWidth(self, text, fontName, fontSize, encoding=None)
            canvas.setPageCompression(self.config.pageCompression)
            #canvas.setPageTransition(self, effectname=None, duration=1, direction=0,dimension='H',motion='I')

            # header
            if self._title:
                canvas.setFont('Times-Roman', 12)
                canvas.drawString(2.0*cm, self.config.pageSize[1] - 1.2*cm, self._title)
            if self._author:
                canvas.setFont('Times-Roman', 10)
                canvas.drawString(2.0*cm, self.config.pageSize[1] - 1.2*cm - 14.0, self._author)
            canvas.line(2.0*cm, self.config.pageSize[1] - 2.0*cm, self.config.pageSize[0] - 2.0*cm, self.config.pageSize[1] - 2.0*cm)

            # body
            #self.reader.drawPageBody(canvas)

            # footer
            canvas.line(2.0*cm, 2.0*cm, self.config.pageSize[0]-2.0*cm, 2.0*cm)
            #if hasattr(canvas, 'headerLine'): # hackish
            #    headerline = string.join(canvas.headerLine, ' \215 ') # bullet
            #    canvas.drawString(2*cm, self.config.pageSize[1]-1.75*cm, headerline)
            canvas.setFont('Times-Roman', 8)
            canvas.drawString(2.0*cm, 1.65*cm, self.config.writeFileName)
            canvas.drawString(2.0*cm, 1.65*cm - 10.0, datetime.datetime.now().strftime("%Y-%m-%d %H:%M"))
            canvas.setFont('Times-Roman', 12)
            pageNumber = canvas.getPageNumber()
            canvas.drawString(4.0*inch, 1.0*cm, "%d" % pageNumber)

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

    class MyParagraph:
        def __init__(self, story, reader, styles):
            self.story = story
            self.reader = reader
            self.styles = styles
            self.styleName = reader.attribute("text:style-name","Standard")
            print "MyParagraph: name=%s namespaceURI=%s level=%s styleName=%s text=%s" % (self.reader.name(),self.reader.namespaceURI(),self.reader.level(),self.styleName,self.reader.text())

            try:
                self.style = self.styles[self.styleName]
            except KeyError:
                parentStyle = self.styles['BodyText']
                self.style = ParagraphStyle(self.styleName, parentStyle)

                kwparagstyle = KWord.paragraphStyle(self.styleName)
                if not kwparagstyle:
                    decodedStyleName = urllib.unquote( self.styleName.replace('_','%') )
                    kwparagstyle = KWord.paragraphStyle(decodedStyleName)
                    if not kwparagstyle:
                        for n in (decodedStyleName, self.styleName):
                            kwparagstyle = KWord.paragraphStyle( re.sub('[0-9]+','',n) )
                            if kwparagstyle:
                                break

                if kwparagstyle:
                    print "PARAG-STYLE=>%s" % kwparagstyle.name()
                    kwcharstyle = kwparagstyle.characterStyle()

                    fontFamily = kwcharstyle.family()
                    if fontFamily:
                        self.style.fontName = fontFamily #'Times-Roman'

                    fontSize = kwcharstyle.size()
                    if fontSize < 6:
                        fontSize = 6
                    self.style.fontSize = fontSize #20

                    #self.style.leading = 12
                    #self.style.leftIndent = 0
                    #self.style.rightIndent = 0
                    #self.style.firstLineIndent = 0
                    #self.style.spaceBefore = 0
                    #self.style.spaceAfter = 0
                    #self.style.bulletFontName = 'Times-Roman'
                    #self.style.bulletFontSize = 10
                    #self.style.bulletIndent = 0
                    #self.style.textColor = black

                    alignment = kwparagstyle.alignment()
                    if alignment == kwparagstyle.AlignLeft:
                        self.style.alignment = TA_LEFT
                    elif alignment == kwparagstyle.AlignHCenter:
                        self.style.alignment = TA_CENTER
                    elif alignment == kwparagstyle.AlignRight:
                        self.style.alignment = TA_RIGHT
                    elif alignment == kwparagstyle.AlignJustify:
                        self.style.alignment = TA_JUSTIFY

                self.styles.add( self.style )

            self.text = ""
            t = self.reader.text()
            if t and not self.reader.hasChildren():
                self.text = t.replace('&','&amp;').replace('"','&quot;').replace('<','&gt;').replace('>','&lt;')

        def __del__(self):
            if self.text:
                #text = "<font size=\"%s\">%s</font>" % (style._size,text)
                self.story.append( Paragraph("%s" % self.text, self.style) )

    class MySpan:
        def __init__(self, myParagraph):
            #self.story = myParagraph.story
            self.reader = myParagraph.reader
            #self.styles = myParagraph.styles
            self.styleName = self.reader.attribute("text:style-name","Standard")
            print "MySpan: name=%s namespaceURI=%s level=%s styleName=%s text=%s" % (self.reader.name(),self.reader.namespaceURI(),self.reader.level(),self.styleName,self.reader.text())

            text = self.reader.text()
            if text:
                text = text.replace('&','&amp;').replace('"','&quot;').replace('<','&gt;').replace('>','&lt;')
                kwcharstyle = self.getCharacterStyle()
                if kwcharstyle:
                    print "CHAR-STYLE=>%s" % kwcharstyle.name()
                    if kwcharstyle.italic():
                        text = "<i>%s</i>" % text
                    if kwcharstyle.bold():
                        text = "<b>%s</b>" % text
                    if kwcharstyle.underline():
                        text = "<u>%s</u>" % text
                myParagraph.text += "%s " % text

        def getCharacterStyle(self):
            kwcharstyle = KWord.characterStyle(self.styleName)
            if not kwcharstyle:
                decodedStyleName = urllib.unquote( self.styleName.replace('_','%') )
                kwcharstyle = KWord.characterStyle(decodedStyleName)
                if not kwcharstyle:
                    for n in (decodedStyleName, self.styleName):
                        kwcharstyle = KWord.characterStyle( re.sub('[0-9]+','',n) )
                        if kwcharstyle:
                            break
            return kwcharstyle

    def __init__(self, config):
        self.config = config
        self.kwdoc = KWord.document()
        self.style = getSampleStyleSheet()
        self.doc = MyWriter.MyTemplate(self)

    def write(self):
        style = getSampleStyleSheet()
        story = []

        kwdoc = KWord.document()
        if not embeddedInKWord and self.config.readOdfFile:
            kwdoc.openUrl(self.config.readOdfFile)

        # Get the KoStore backend for the file.
        store = KWord.store()

        # We like to read the content.xml file from the KoStore.
        reader = store.open("content.xml")
        if not reader:
            raise "Failed to read file from the store"

        self._myParagraph = None

        # This method got called on each readed element.
        def onElement():
            if reader.isNull():
                return
            if reader.name() == "text:p":
                self._myParagraph = MyWriter.MyParagraph(story, reader, self.style)
            elif reader.name() == "text:span":
                myspan = MyWriter.MySpan(self._myParagraph)
            elif reader.name() == "text:s":
                #story.append( Spacer(1.0*inch, 0.0*inch) )
                pass
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

        #story.append( Paragraph('<font size=18>Some Text</font>', style['BodyText']) )
        #story.append( PageBreak() )
        self.doc.multiBuild(story)

class MyDialog:
    def __init__(self, action, config):
        self.config = config

        forms = Kross.module("forms")
        self.dialog = forms.createDialog("ReportLab.org")
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

if embeddedInKWord or not config.writeFileName:
    config.showDialog = True

if config.showDialog:
    MyDialog(self, config)
else:
    writer = MyWriter(config)
    writer.write()
