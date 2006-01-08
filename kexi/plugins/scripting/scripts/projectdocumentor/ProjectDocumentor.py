""" 
Project Documentor

Description:
This script collects various informations about a Kexi project
and exports them to a HTML file.

Author:
Sebastian Sauer <mail@dipe.org>

Copyright:
Dual-licensed under LGPL v2+higher and the BSD license.
"""

class DataProvider:
	def __init__(self):
		import kexiapp
		keximainwindow = kexiapp.get("KexiAppMainWindow")

		try:
			self.connection = keximainwindow.getConnection()
		except:
			raise "No connection established. Please open the project to be documented first."

	def printConnection(self):
		condata = self.connection.data()
		infos = []
		for item in ("caption", "description", "driverName", "hostName", "port", "userName", "fileName", "dbPath", "localSocketFileName", "serverInfoString"):
			result = getattr(condata, item)()
			if result != None and result != "" and (item != "port" or result != 0):
				infos.append( (item, result) )
		return infos

	def printDriver(self):
		driver = self.connection.driver()
		result = [ ("Version", "%s.%s" % (driver.versionMajor(),driver.versionMinor())) ]
		conlist = driver.connectionsList()
		if len(conlist) > 0:
			result.append( ("Connections",str(conlist)) )
		return result

	def printDatabases(self):
		result = [ ("Current database", self.connection.currentDatabase()) ]
		dbnames = self.connection.databaseNames()
		if len(dbnames) > 0:
			result.append( ("Databases",str(dbnames)) )
		return result

	def printTables(self):
		result = []
		for t in self.connection.tableNames():
			tableschema = self.connection.tableSchema(t)
			ti = []
			for i in ("name", "caption", "description"):
				v = getattr(tableschema,i)()
				if v != None and v != "":
					ti.append( (i,v) )
			tf = []
			for field in tableschema.fieldlist().fields():
				tfi = []
				for n in ("caption","description","type","subType","typeGroup","length","defaultValue"):
					v = getattr(field,n)()
					if v != None and v != "":
						tfi.append( (n,v) )
				props = []
				for n in ("PrimaryKey","ForeignKey","AutoInc","UniqueKey","NotNull", "NotEmpty","Indexed","Unsigned"):
					v = getattr(field,"is%s" % n)()
					if v != None and v != "" and v != False and v != 0:
						props.append( "%s " % n )
				if len(props) > 0:
					tfi.append( ("properties",props) )

				tf.append( (field.name(), tfi) )
			ti.append( ("fields", tf) )
			if len(ti) > 0:
				result.append( (t, ti) )
		return result

	def printQueries(self):
		result = []
		for q in self.connection.queryNames():
			queryschema = self.connection.querySchema(q)
			qi = []
			for i in ("name", "caption", "description", "statement"):
				v = getattr(queryschema,i)()
				if v != None and v != "":
					qi.append( (i,v) )
			if len(qi) > 0:
				result.append( (q, qi) )
		return result

class GuiApp:
	def __init__(self, dataprovider):
		self.dataprovider = dataprovider

		try:
			import gui
		except:
			raise "Import of the Kross GUI module failed."

		self.dialog = gui.Dialog("Project Documentor")

		self.dialog.addLabel(self.dialog, "Save information about the project to an HTML file.")
		
		self.file = self.dialog.addFileChooser(self.dialog,
			"File:",
			gui.getHome() + "/projectdoc.html",
			(('HTML files', '*.html'),('All files', '*')))

		self.printCheckBoxes = {}
		for d in dir(self.dataprovider):
			if d.startswith("print"):			
				self.printCheckBoxes[d] = self.dialog.addCheckBox(self.dialog, d[5:], True)
				
				#value = getattr(self.dataprovider,d)()
				#if value != None and len(value) > 0:
				#	f.write("<h2>%s</h2>" % d[5:])
				#	f.write( self.toHTML(value) )

		#self.exportProjectdetails = 
		#self.exportTableschemas = self.dialog.addCheckBox(self.dialog, "Table schemas", True)
		#self.exportQueryschemas = self.dialog.addCheckBox(self.dialog, "Query schemas", True)

		btnframe = self.dialog.addFrame(self.dialog)
		self.dialog.addButton(btnframe, "Save", self.doSave)
		self.dialog.addButton(btnframe, "Cancel", self.dialog.close)

		self.dialog.show()

	def toHTML(self, value):
		import types
		result = ""
		if isinstance(value, types.TupleType):
			result += "<ul>"
			if len(value) == 1:
				result += "<li>%s</li>" % value
			elif len(value) == 2:
				result += "<li>%s: %s</li>" % (value[0], self.toHTML(value[1]))
			elif len(value) > 2:
				for item in value:
					i = self.toHTML(item)
					if i != "":
						result += "<li>%s</li>" % i
			result += "</ul>"
		elif isinstance(value, types.ListType):
			for item in value:
				result += "%s" % self.toHTML(item)
		else:
			result += "%s" % value
		return result

	def doSave(self):
		file = str( self.file.get() )
		print "Attempting to save project documentation to file: %s" % file

		f = open(file, "w")

		f.write("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>")
		f.write("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 4.01 Strict//EN\" \"http://www.w3.org/TR/xhtml11/DTD/xhtml11-strict.dtd\">")
		f.write("<html><head><title>Project information</title>")
		f.write("<style type=\"text/css\">")
		f.write("  html { background-color:#fafafa; }")
		f.write("  body { background-color:#ffffff; margin:1em; padding:1em; border:#99a 1px solid; color:#003; }")
		f.write("</style>")
		f.write("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />")
		f.write("</head><body><h1>Project information</h1>")

		for d in dir(self.dataprovider):
			if d.startswith("print"):
				print "GuiApp.doSave() CHECK %s" % d
				a = self.printCheckBoxes[d]
				if a and a.isChecked():
					print "GuiApp.doSave() BEGIN %s" % d
					value = getattr(self.dataprovider,d)()
					if value != None and len(value) > 0:
						f.write("<h2>%s</h2>" % d[5:])
						f.write( self.toHTML(value) )
					print "GuiApp.doSave() END %s" % d

		f.close()

		print "Successfully saved project documentation to file: %s" % file
		self.dialog.close()

GuiApp( DataProvider() )

