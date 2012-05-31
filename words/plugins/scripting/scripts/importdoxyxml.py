#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
This Words python script implements import of with doxygen generated content into Words.

To generate the handbook from within the commandline;
    cd kspread/plugins/scripting/docs
    doxygen kspreadscripting.doxyfile
    cd xml
    xsltproc combine.xslt index.xml | ../doxy2doc.py ../kspread.html

To import the handbook in Words, first generate the doxygen xml file like demonstrated
bellow and then use in Words the "Import Doxygen XML File" python script;
    cd kspread/plugins/scripting/docs
    doxygen kspreadscripting.doxyfile
    cd xml
    xsltproc combine.xslt index.xml > ~/mydoxygen.xml
"""

import os, sys, re, traceback, xml.dom.minidom

class Config:
    """ The configuration. """

    def __init__(self):
        self.FileName = ""
        self.TableOfContent = True
        self.EnablePages = True
        self.IndexPage = True
        self.OtherPages = True
        self.PageDescription = True
        self.EnableClasses = True
        self.ClassDescription = True
        self.MemberDescription = True
        self.EnableSlots = True
        self.EnableSignals = True
        self.EnableProperties = True
        self.EnableFunctions = False
        self.EnableVariables = False

class Class:
    """ Provides us details about a class and it's members like slots, signals, properties, etc. """

    class Slot:
        def __init__(self, id, node):
            self.id = id
            self.node = node
            self.kindName = "slot"

            self.description = node.getElementsByTagName("detaileddescription")[0].toxml()

            d = node.getElementsByTagName("definition")[0].childNodes[0].data #e.g. "virtual QString sheet"
            d = d.replace("virtual ","")
            a = node.getElementsByTagName("argsstring")[0].childNodes[0].data #e.g. "(const QString &amp;name)"
            a = re.sub("=[\s]*0$","",a)
            a = re.sub("(^|[^a-zA-Z0-9])const($|[^a-zA-Z0-9])", "\\1\\2", "%s%s" % (d,a))
            a = re.sub("&|\*","",a)
            a = re.sub("[\s]*(\(|\))[\s]*","\\1",a)
            self.definition = a.strip()

    class Signal:
        def __init__(self, id, node):
            self.id = id
            self.node = node
            self.kindName = "signal"

            self.description = node.getElementsByTagName("detaileddescription")[0].toxml()

            d = node.getElementsByTagName("definition")[0].childNodes[0].data #e.g. "void changedSheet"
            d = d.replace("virtual ","")
            a = node.getElementsByTagName("argsstring")[0].childNodes[0].data #e.g. "(const QString &amp;name)"
            a = re.sub("=[\s]*0$","",a)
            a = re.sub("(^|[^a-zA-Z0-9])const($|[^a-zA-Z0-9])", "\\1\\2", "%s%s" % (d,a))
            a = re.sub("&|\*","",a)
            a = re.sub("[\s]*(\(|\))[\s]*","\\1",a)
            self.definition = a.strip()
            #print node.toxml()

    class Property:
        def __init__(self, id, node):
            self.id = id
            self.node = node

            read = len(node.getElementsByTagName("read")) > 0
            write = len(node.getElementsByTagName("write")) > 0
            if read and write:
                self.kindName = "property, read+write"
            elif read:
                self.kindName = "property, read only"
            elif write:
                self.kindName = "property, write only"
            else:
                self.kindName = "property"

            self.description = node.getElementsByTagName("detaileddescription")[0].toxml()
            d = node.getElementsByTagName("definition")[0].childNodes[0].data #e.g. "QString error"
            d = d.replace("const ","")
            d = re.sub("&|\*","",d)
            self.definition = d.strip()
            #print node.toxml()

    class Variable:
        def __init__(self, id, node):
            self.id = id
            self.node = node
            self.kindName = "variable"
            self.description = node.getElementsByTagName("detaileddescription")[0].toxml()
            d = node.getElementsByTagName("definition")[0].childNodes[0].data #e.g. "QString error"
            d = d.replace("const ","")
            d = re.sub("&|\*","",d)
            self.definition = d.strip()
            #print node.toxml()

    class Function:
        def __init__(self, id, node):
            print ""
            self.id = id
            self.node = node
            self.kindName = "function"
            self.description = node.getElementsByTagName("detaileddescription")[0].toxml()
            d = node.getElementsByTagName("definition")[0].childNodes[0].data #e.g. "virtual QString sheet"
            d = d.replace("virtual ","")
            a = node.getElementsByTagName("argsstring")[0].childNodes[0].data #e.g. "(const QString &amp;name)"
            a = re.sub("=[\s]*0$","",a)
            a = re.sub("(^|[^a-zA-Z0-9])const($|[^a-zA-Z0-9])", "\\1\\2", "%s%s" % (d,a))
            a = re.sub("&|\*","",a)
            a = re.sub("[\s]*(\(|\))[\s]*","\\1",a)
            self.definition = a.strip()
            #print node.toxml()

    def __init__(self, config, node):
        self.node = node
        self.description = " ".join( [ n.toxml() for n in node.childNodes if n.nodeName == "detaileddescription" ] )
        self.memberDict = {}
        self.memberList = []
        for n in self.node.getElementsByTagName("memberdef"):
            id = n.getAttribute("id")
            kind = n.getAttribute("kind")
            if kind == "slot":
                if config.EnableSlots:
                    self.memberDict[id] = Class.Slot(id, n)
                    self.memberList.append(id)
            elif kind == "signal":
                if config.EnableSignals:
                    self.memberDict[id] = Class.Signal(id, n)
                    self.memberList.append(id)
            elif kind == "property":
                if config.EnableProperties:
                    self.memberDict[id] = Class.Property(id, n)
                    self.memberList.append(id)
            elif kind == "function":
                if config.EnableFunctions:
                    self.memberDict[id] = Class.Function(id, n)
                    self.memberList.append(id)
            elif kind == "variable":
                if config.EnableVariables:
                    self.memberDict[id] = Class.Variable(id, n)
                    self.memberList.append(id)
            else:
                print "  Skipping class-member id=%s kind=%s" % (id, kind)
                #print n.toxml()

class Page:
    """ A doxygen page. """

    def __init__(self, config, node):
        print "PAGE -------------------------------------\n"
        print node.toxml()
        self.title = node.getElementsByTagName("title")[0].childNodes[0].data #e.g. "KSpread Scripting Plugin"
        self.description = " ".join( [ n.toxml() for n in node.childNodes if n.nodeName == "detaileddescription" ] )

class Writer:
    """ The writer parses the doxygen XML and writes it to e.g. HTML. """

    def __init__(self, doc, config):
        self.doc = doc
        self.config = config
        self.CompoundDict = {}
        self.CompoundList = []

        for node in doc.getElementsByTagName("compounddef"):
            id = node.getAttribute("id")
            kind = node.getAttribute("kind") #e.g. "class"
            #prot = node.getAttribute("prot") #e.g. "public"
            name = node.getElementsByTagName("compoundname")[0].childNodes[0].data #e.g. "ScriptingPart"

            if name.startswith('Q') or name.startswith('KPart'):
                continue

            if kind == "class":
                if not self.config.EnableClasses:
                    continue
                impl = Class(self.config, node)
            elif kind == "page":
                if not self.config.EnablePages:
                    continue
                impl = Page(self.config, node)
            else:
                print "Skipping id=%s name=%s kind=%s" % (id, name, kind)
                #raise RuntimeError("Unknown kind '%s'" % kind)
                continue
            impl.id = id
            impl.name = name
            impl.kind = kind
            self.CompoundDict[id] = impl
            self.CompoundList.append(id)

    def writeHtml(self, file):
        try:
            title = self.CompoundDict["indexpage"].title
        except KeyError:
            #print "".join( traceback.format_exception(sys.exc_info()[0],sys.exc_info()[1],sys.exc_info()[2]) )
            title = "Doxygen XML"

        class HtmlParser:
            def __init__(self, writer): self.writer = writer
            def para(self, m): return "<%sp%s>" % (m.group(1),m.group(3))
            def sp(self, m): return "&nbsp;"
            def programlisting(self, m): return "<%spre%s>" % (m.group(1),m.group(3))
            def listitem(self, m): return "<%sli%s>" % (m.group(1),m.group(3))
            def ref(self, m):
                if m.group().__contains__('refid'):
                    refid = re.search("refid=\"(.*?)\"",m.group()).group(1)
                    if self.writer.CompoundDict.__contains__(refid):
                        if self.writer.CompoundDict[refid].kind in ["page","class"]:
                            return "<a href=\"#%s\">" % refid
                return "<%sa%s>" % (m.group(1),m.group(3))
                #raise AttributeError, "AAAAAAAAAAAAAAAAAAAAAA"
            def title(self, m):
                return "<%sh3%s>" % (m.group(1),m.group(3))

        parser = HtmlParser(self)
        def htmlReplacer( match ):
            tagname = match.group(2).strip()
            try:
                tagname = tagname[ : tagname.index(' ') ]
            except ValueError:
                pass
            if hasattr(parser,tagname):
                return getattr(parser,tagname)(match)
            #raise AttributeError, tagname
            return ""

        def parseToHtml( xmlstring ):
            xmlstring = re.compile( "<([\/\s]*)(.*?)([\/\s]*)>" ).sub(htmlReplacer, xmlstring)
            xmlstring = re.sub("(?<!\")((http|https|ftp)://[a-zA-Z0-9\.\_\-\;\?\&\/\=]*)", "<a href=\"\\1\">\\1</a>", xmlstring)
            xmlstring = re.sub("<li>[\s]*<p>","<li>", xmlstring)
            xmlstring = re.sub("</p>[\s]*</li>","</li>", xmlstring)
            return xmlstring

        file.write( "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n" )
        file.write( "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\" \"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">\n" )
        file.write( "<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"en\">\n" )
        file.write( "<head><title>%s</title>" % title )

        #file.write( "<style type=\"text/css\">" )
        #file.write( "<!--" )
        #file.write( "HTML { background-color:#eef; } ")
        #file.write( "BODY { margin:1em; border:1em; padding:1em; font-size:100%; color:#003; background-color:#fff; border:#99a 1px solid; } ")
        #file.write( "H1 { margin:0em 0em 1em 0em; font-size:1.5em; color:#009; text-align:center; } ")
        #file.write( "H2 { margin:0em 0em 1em 0em; font-size:1.3em; color:#009; border-bottom:#699 1px dotted; } ")
        #file.write( "H3,H4,H5,H6 { margin:2em 0em 1em 1em; font-size:1.1em; color:#009; border-bottom:#699 1px dotted; } ")
        #file.write( "PRE { margin-left:1em; padding:0.5em; background-color:#f3f3ff; } ")
        #file.write( ".member { margin:0.5em; background-color:#f3f3ff; } ")
        #file.write( "//-->" )
        #file.write( "</style>" )

        file.write( "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />\n" )
        file.write( "</head><body><h1>%s</h1>\n" % title )

        if self.config.TableOfContent:
            file.write("<ol>")

            if self.config.EnablePages:
                file.write("<li><a href=\"#pages\">Pages</a><ol>")
                for i in self.CompoundList:
                    if self.CompoundDict[i].kind != "page":
                        continue
                    if i == "indexpage":
                        if not self.config.IndexPage:
                            continue
                    else:
                        if not self.config.OtherPages:
                            continue
                    file.write( "<li><a href=\"#%s\">%s</a></li>" % (i, self.CompoundDict[i].name) )
                file.write("</ol></li>")

            if self.config.EnableClasses:
                file.write("<li><a href=\"#objects\">Classes</a><ol>")
                for i in self.CompoundList:
                    if self.CompoundDict[i].kind != "class":
                        continue
                    file.write( "<li><a href=\"#%s\">%s</a></li>" % (i, self.CompoundDict[i].name) )
                file.write("</ol></li>")

            file.write("</ol>")

        # Pages
        if self.config.EnablePages:
            if self.config.IndexPage:
                try:
                    file.write("<h2><a name=\"indexpage\" />%s</h2>" % self.CompoundDict["indexpage"].name)
                    file.write( parseToHtml( self.CompoundDict["indexpage"].description ) )
                except KeyError:
                    pass
            if self.config.OtherPages:
                for i in self.CompoundList:
                    if self.CompoundDict[i].kind != "page" or i == "indexpage":
                        continue
                    file.write("<h3><a name=\"%s\" />%s</h3>" % (i,self.CompoundDict[i].name))
                    if self.config.PageDescription:
                        file.write( "%s<br />" % parseToHtml( self.CompoundDict[i].description ) )

        # Classes
        if self.config.EnableClasses:
            file.write("<h2><a name=\"objects\" />Classes</h2>")
            for i in self.CompoundList:
                if self.CompoundDict[i].kind != "class":
                    continue
                file.write("<h3><a name=\"%s\" />%s</h3>" % (i,self.CompoundDict[i].name))
                if self.config.ClassDescription:
                    file.write( "%s<br />" % parseToHtml( self.CompoundDict[i].description ) )
                file.write( "<ul>" )
                for m in self.CompoundDict[i].memberList:
                    member = self.CompoundDict[i].memberDict[m]
                    s = "%s <i>[%s]</i>" % (member.definition,member.kindName)
                    if self.config.MemberDescription:
                        if len(member.description) > 0:
                            s += "<br><blockquote>%s</blockquote>" % parseToHtml( member.description )
                    file.write("<li>%s</li>" % s)
                file.write( "</ul>" )

        file.write("</body></html>")

class ImportDialog:
    """ The dialog we are using to provide some frontend to the user if the script runs
    for example embedded in Words. """

    def __init__(self, action, config):
        import Kross, Words

        self.action = action
        self.config = config

        forms = Kross.module("forms")
        self.dialog = forms.createDialog("Import Doxygen XML File")
        self.dialog.setButtons("Ok|Cancel")
        self.dialog.setFaceType("List") #Auto Plain List Tree Tabbed

        openpage = self.dialog.addPage("Open","Import Doxygen XML File","document-open")
        openwidget = forms.createFileWidget(openpage, "kfiledialog:///wordssampleimportfile")
        openwidget.setMode("Opening")
        #openwidget.minimumWidth = 540
        #openwidget.minimumHeight = 400
        openwidget.setFilter("*.xml|XML Files\n*|All Files")

        configpage = self.dialog.addPage("Options","Import Options","preferences-other")
        configwidget = forms.createWidgetFromUIFile(configpage, os.path.join(action.currentPath(),"importdoxyxmloptions.ui"))

        stylepage = self.dialog.addPage("Styles","Cascading Style Sheet","preferences-web-browser-stylesheets")
        stylewidget = forms.createWidgetFromUIFile(stylepage, os.path.join(action.currentPath(),"importdoxyxmlstyle.ui"))
        styleedit = stylewidget["textEdit"]

        if self.dialog.exec_loop():
            fileName = openwidget.selectedFile()
            if not fileName:
                raise "No file selected"

            self.config.FileName = fileName
            self.config.TableOfContent = configwidget["TocCheckBox"].checked
            self.config.IndexPage = configwidget["IndexPageCheckBox"].checked
            self.config.OtherPages = configwidget["OtherPagesCheckBox"].checked
            self.config.EnablePages = configwidget["PagesCheckBox"].checked
            self.config.PageDescription = configwidget["PageDescriptionCheckBox"].checked
            self.config.EnableClasses = configwidget["ClassesCheckBox"].checked
            self.config.ClassDescription = configwidget["ClassDescriptionCheckBox"].checked
            self.config.MemberDescription = configwidget["MemberDescriptionCheckBox"].checked
            self.config.EnableSlots = configwidget["SlotsCheckBox"].checked
            self.config.EnableSignals = configwidget["SignalsCheckBox"].checked
            self.config.EnableProperties = configwidget["PropertiesCheckBox"].checked
            self.config.EnableFunctions = configwidget["FunctionsCheckBox"].checked
            self.config.EnableVariables = configwidget["VariablesCheckBox"].checked

            self.importFile(styleedit.plainText)

    def __del__(self):
        self.dialog.delayedDestruct()

    def importFile(self, styles):
        import Kross, Words

        try:
            file = open(self.config.FileName, "r")
        except IOError, (errno, strerror):
            raise "Failed to read file \"%s\":\n%s" % (self.config.FileName, strerror)

        xmldoc = xml.dom.minidom.parse( file )

        kwdoc = Words.mainFrameSet().document()
        kwdoc.setDefaultStyleSheet("%s" % styles)

        class WordsFileWriter:
            def __init__(self, kwdoc):
                self.kwdoc = kwdoc
                self.lines = []
            def write(self, line):
                self.lines.append( line.encode('utf-8') )
            def flush(self):
                self.kwdoc.setHtml(' '.join(self.lines))
        writer = Writer(xmldoc, self.config)
        kwwriter = WordsFileWriter(kwdoc)
        writer.writeHtml(kwwriter)
        kwwriter.flush()

if __name__=="__main__":
    # this means we are running direct with the python interpreter.
    if len(sys.argv) != 2:
        print "%s outputfilename.xml" % sys.argv[0]
        sys.exit()

    try:
        outputfilename = sys.argv[1]
        file = open(outputfilename, "w")
    except IOError, (errno, strerror):
        raise "Failed to create file \"%s\":\n%s" % (filename, strerror)
    xmldoc = xml.dom.minidom.parse( sys.stdin )
    writer = Writer(xmldoc, Config())
    writer.writeHtml(file)

else:
    # We are probably embedded into e.g. Words. But in any case we display
    # the import-dialog to let the user choose what he wanna do.
    import Kross
    ImportDialog(self, Config())
