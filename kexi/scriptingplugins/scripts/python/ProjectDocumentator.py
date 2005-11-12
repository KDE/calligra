""" 
Project Documentator

Description:
This script collects various informations about a Kexi project
and exports them to a HTML file.

Author:
Sebastian Sauer <mail@dipe.org>

Copyright:
Published as-is without any warranties.
"""

class DataProvider:
	def __init__(self):
		try:
			import krosskexiapp
			keximainwindow = krosskexiapp.get("KexiAppMainWindow")
		except:
			raise "Import of the Kross KexiApp module failed."

		try:
			self.connection = keximainwindow.getConnection()
		except:
			raise "No connection established. Please open the project that should be documentated before."

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
		conlist = driver.connectionList()
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

		self.dialog = gui.Dialog("Project Documentator")
		self.dialog.addLabel(self.dialog.addFrame(self.dialog), "Export informations about the opened project.")
		#self.dialog.addCheckbox(self.dialog.addFrame(self.dialog), "Projectdetails")
		#self.dialog.addCheckbox(self.dialog.addFrame(self.dialog), "Tableschemas")
		#self.dialog.addCheckbox(self.dialog.addFrame(self.dialog), "Queryschemas")
		#self.dialog.addCheckbox(self.dialog.addFrame(self.dialog), "Tablestatistics")
		#self.dialog.addCheckbox(self.dialog.addFrame(self.dialog), "Relationships")

		self.file = self.dialog.addFileChooser(self.dialog,
			"Save as:",
			gui.getHome() + "/output.html",
			(('HTML files', '*.html'),('All files', '*')))

		btnframe = self.dialog.addFrame(self.dialog)
		self.dialog.addButton(btnframe, "Save", self.doSave)
		self.dialog.addButton(btnframe, "Cancel", self.dialog.quitDialog)

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
		file = self.file.get()
		print "Try to save project documentation to file: %s" % file

		f = open(file, "w")

		f.write("<html><head><title>Projectinformations</title></head><body>")
		f.write("<h1>Projectinformations</h1>")

		for d in dir(self.dataprovider):
			if d.startswith("print"):
				value = getattr(self.dataprovider,d)()
				if value != None and len(value) > 0:
					f.write("<h2>%s</h2>" % d[5:])
					f.write( self.toHTML(value) )

		f.close()

		print "Successfully saved project documentation to file: %s" % file
		self.dialog.quitDialog()

GuiApp( DataProvider() )

