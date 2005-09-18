""" Export to HTML

    Description:
    This script exports a KexiDB table or query to a HTML file.

    Author:
    Sebastian Sauer <mail@dipe.org>

    Copyright:
    Published as-is without any warranties.
"""

class Datasource:
    def __init__(self):
        try:
            import krosskexiapp
            keximainwindow = krosskexiapp.get("KexiAppMainWindow")
        except:
            raise "Import of the Kross KexiApp module failed."

        try:
            self.connection = keximainwindow.getConnection()
        except:
            raise "No connection established. Please open a project before."

        self.queryschema = None

    def getSources(self):
        sources = [""]
        for table in self.connection.tableNames():
            sources.append("Tables/%s" % table)
        for query in self.connection.queryNames():
            sources.append("Queries/%s" % query)
        sources.sort()
        return sources

    def setSource(self, source):
        s = source.split("/",1)
        if s[0] == "Tables":
            self.queryschema = self.connection.tableSchema( s[1] )
            if self.queryschema != None: # we need a queryschema not a tableschema
                self.queryschema = self.queryschema.query() # "select * from table"-query
        elif s[0] == "Queries":
            self.queryschema = self.connection.querySchema( s[1] )
        self.cursor = None
        return self.queryschema != None

    def name(self):
        return self.queryschema.name()

    def caption(self):
        return self.queryschema.caption()

    def description(self):
        return self.queryschema.description()

    def header(self):
        h = []
        for field in self.queryschema.fieldlist().fields():
            h.append( field.name() ) #field.caption()
        return h

    def getNext(self):
        if not self.cursor:
            self.cursor = self.connection.executeQuerySchema( self.queryschema )
            if not self.cursor:
                raise "Failed to execute queryschema."
            if not self.cursor.moveFirst():
                raise "Failed to move cursor to first record."
        if self.cursor.eof():
            self.cursor = None
            return None
        items = []
        for i in range( self.cursor.fieldCount() ):
            items.append( self.cursor.value(i) )
        self.cursor.moveNext()
        return items

class HtmlExporter:
    def __init__(self, datasource):
        self.datasource = datasource

    def write(self, output):
        name = self.datasource.name()
        output.write("<html><head><title>%s</title></head><body>" % name)
        output.write("<h1>%s</h1>" % name)

        caption = self.datasource.caption()
        if caption and caption != name:
            output.write("caption: %s<br />" % caption)

        description = self.datasource.description()
        if description:
            output.write("description: %s<br />" % description)

        #import datetime
        #output.write("date: %s<br />" % datetime.datetime.now())

        output.write("<table border='1'>")

        #output.write("<tr>")
        #for h in self.datasource.header():
        #    output.write("<th>%s</th>" % h)
        #output.write("</tr>")

        while 1 == 1:
            items = self.datasource.getNext()
            if items == None: break
            output.write("<tr>")
            for item in items:
                output.write("<td>%s</td>" % item)
            output.write("</tr>")
        output.write("</table>")
        output.write("</body></html>")

class GuiApp:
    def __init__(self, datasource):
        self.datasource = datasource

        try:
            import Tkinter
        except:
            raise "Import of the python TKinter module failed."

        self.root = Tkinter.Tk()
        self.root.title("Export to HTML")

        frame = Tkinter.Frame(self.root)
        frame.pack()

        queryframe = Tkinter.Frame(frame)
        queryframe.pack()
        Tkinter.Label(queryframe, text="Table or query to export:").pack(side=Tkinter.LEFT)
        self.querycontent = Tkinter.StringVar()
        self.query = apply(Tkinter.OptionMenu,
                          (queryframe, self.querycontent) + tuple( self.datasource.getSources() ))
        self.query.pack(side=Tkinter.LEFT)

        fileframe = Tkinter.Frame(frame)
        fileframe.pack()
        Tkinter.Label(fileframe, text="Export to file:").pack(side=Tkinter.LEFT)
        self.filecontent = Tkinter.StringVar()
        file = Tkinter.Entry(fileframe, width=36, textvariable=self.filecontent)
        self.filecontent.set(self.getHome() + "/output.html")
        file.pack(side=Tkinter.LEFT)
        Tkinter.Button(fileframe, text="...",
                       command=self.doBrowse).pack(side=Tkinter.LEFT)

        btnframe = Tkinter.Frame(frame)
        btnframe.pack()
        Tkinter.Button(btnframe, text="Export",
                       command=self.doExport).pack(side=Tkinter.LEFT)
        Tkinter.Button(btnframe, text="Cancel",
                       command=self.root.destroy).pack(side=Tkinter.LEFT)

        self.root.mainloop()

    def getHome(self):
        import os
        try:
            home = os.getenv("HOME")
            if not home:
                import pwd
                user = os.getenv("USER") or os.getenv("LOGNAME")
                if not user:
                    pwent = pwd.getpwuid(os.getuid())
                else:
                    pwent = pwd.getpwnam(user)
                home = pwent[6]
            return home
        except (KeyError, ImportError):
            return os.curdir

    def doBrowse(self):
        import tkFileDialog
        file = tkFileDialog.asksaveasfilename(initialdir=self.getHome(),
            initialfile='output.html', defaultextension='.html',
            filetypes=(('HTML files', '*.html'),('All files', '*')))
        if file: self.filecontent.set( file )

    def doExport(self):
        query = self.querycontent.get()
        if not self.datasource.setSource(query):
            import tkMessageBox
            tkMessageBox.showerror("Error", "Please select a valid table or query.")
            return

        file = self.filecontent.get()
        print "Exporting '%s' to file '%s' ..." % (query,file)

        f = open(file, "w")
        global HtmlExporter
        exporter = HtmlExporter(self.datasource)
        exporter.write(f)
        f.close()

        print "Successfully exported '%s' to file %s" % (query,file)
        self.root.destroy()

GuiApp( Datasource() )
