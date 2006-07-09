"""
Export table or query data.

Description:
This script exports a KexiDB table or query to different fileformats.

Author:
Sebastian Sauer <mail@dipe.org>

Copyright:
Dual-licensed under LGPL v2+higher and the BSD license.
"""

class Datasource:
	def __init__(self):
		import kexiapp
		keximainwindow = kexiapp.get("KexiAppMainWindow")

		try:
			self.connection = keximainwindow.getConnection()
		except:
			raise "No connection established. Please open a project before."

		self.schema = None

	def getSources(self):
		sources = []
		for table in self.connection.tableNames():
			sources.append("Tables/%s" % table)
		for query in self.connection.queryNames():
			sources.append("Queries/%s" % query)
		sources.sort()
		return sources

	def setSource(self, source):
		s = source.split("/",1)
		if s[0] == "Tables":
			self.schema = self.connection.tableSchema( s[1] )
			self.queryschema = self.schema.query()
		elif s[0] == "Queries":
			self.schema = self.connection.querySchema( s[1] )
			self.queryschema = self.schema
		self.cursor = None
		return self.schema != None

	def name(self):
		return self.schema.name()

	def caption(self):
		return self.schema.caption()

	def description(self):
		return self.schema.description()

	def header(self):
		h = []
		for field in self.schema.fieldlist().fields():
			s = field.caption()
			if s == None or s == "":
				s = field.name()
			h.append(s)
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

	def htmlescape(self, text):
		import string
		return string.replace(string.replace(string.replace(str(text),'&','&amp;'),'<','&lt;'),'>','&gt;')

	def write(self, output, style):
		name = self.datasource.name()
		
		output.write("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n")
		output.write("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\" \"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">\n")
		output.write("<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"en\">\n")
		output.write("<head><title>%s</title>\n" % name)
		output.write("<style type=\"text/css\">\n<!--\n")
		if style == "Paper":
			output.write("html { background-color:#efefef; }")
			output.write("body { background-color:#fafafa; color:#303030; margin:1em; padding:1em; border:#606060 1px solid; }")
		elif style == "Blues":
			output.write("html { background-color:#0000aa; }")
			output.write("body { background-color:#000066; color:#efefff; margin:1em; padding:1em; border:#00f 1px solid; }")
			output.write("h1 { color:#0000ff; }")
			output.write("th { color:#0000aa; }")
		else:
			output.write("html { background-color:#ffffff; color:#000; }")
			output.write("body { margin:1em; }")
		output.write("\n//-->\n</style>\n")
		output.write("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />\n")
		output.write("</head><body><h1>%s</h1>\n" % name)

		caption = self.datasource.caption()
		if caption and caption != name:
			output.write("caption: %s<br />\n" % caption)

		description = self.datasource.description()
		if description:
			output.write("description: %s<br />\n" % description)

		#import datetime
		#output.write("date: %s<br />" % datetime.datetime.now())

		output.write("<table border='1'>\n")

		output.write("<tr>")
		for h in self.datasource.header():
			output.write("<th>%s</th>" % h)
		output.write("</tr>")

		while 1 == 1:
			items = self.datasource.getNext()
			if items == None: break
			output.write("<tr>")
			for item in items:
				u = unicode(str(self.htmlescape(item)),"latin-1")
				output.write("<td>%s</td>" % u.encode("utf-8"))
			output.write("</tr>\n")
		output.write("</table>\n")
		output.write("</body></html>\n")

class GuiApp:
	def __init__(self, datasource):
		self.datasource = datasource

		try:
			import gui
		except:
			raise "Import of the Kross GUI module failed."

		self.dialog = gui.Dialog("Export XHTML")
                self.dialog.addLabel(self.dialog, "Export a table- or query-datasource to a XHTML-file.")

		datasourceitems = self.datasource.getSources()
                self.datasourcelist = self.dialog.addList(self.dialog, "Datasource:", datasourceitems)

		styleitems = ["Plain", "Paper", "Blues"]
                self.stylelist = self.dialog.addList(self.dialog, "Style:", styleitems)

		#queryframe = Tkinter.Frame(frame)
		#queryframe.pack()
		#Tkinter.Label(queryframe, text="Table or query to export:").pack(side=Tkinter.LEFT)
		#self.querycontent = Tkinter.StringVar()
		#self.query = apply(Tkinter.OptionMenu, (queryframe, self.querycontent) + tuple( self.datasource.getSources() ))
		#self.query.pack(side=Tkinter.LEFT)

		self.file = self.dialog.addFileChooser(self.dialog,
			"File:",
			gui.getHome() + "/kexidata.xhtml",
			(('XHTML files', '*.xhtml'),('All files', '*')))

		btnframe = self.dialog.addFrame(self.dialog)
		self.dialog.addButton(btnframe, "Export", self.doExport)
		self.dialog.addButton(btnframe, "Cancel", self.dialog.close)

		self.dialog.show()

	def doExport(self):
		file = str( self.file.get() )
		query = str( self.datasourcelist.get() )
		print "Exporting '%s' to file '%s' ..." % (query,file)

		if not self.datasource.setSource(query):
			raise "Invalid datasource selected."
			#return

		style = str( self.stylelist.get() )

		f = open(file, "w")
		global HtmlExporter
		exporter = HtmlExporter(self.datasource)
		exporter.write(f, style)
		f.close()

		print "Successfully exported '%s' to file %s" % (query,file)
		self.dialog.close()

GuiApp( Datasource() )
