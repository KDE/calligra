"""
Import data from a XHTML file to a KexiDB table.

Description:
This script implements import of data from a XHTML file to a KexiDB table. The
table needs to be an already existing table the data should be added to.

Author:
Sebastian Sauer <mail@dipe.org>

Copyright:
Dual-licensed under LGPL v2+higher and the BSD license.
"""

class SaxInput:
	""" The inputsource we like to import the data from. This class
	provides us abstract access to the SAX XML parser we use internaly
	to import data from the XML-file. """

	xmlfile = None
	""" The XML file we should read the content from. """

	def __init__(self):
		""" Constructor. """

		# try to import the xml.sax python module.
		try:
			import xml.sax.saxlib
			import xml.sax.saxexts
		except:
			raise "Import of the python xml.sax.saxlib module failed. This module is needed by the ImportXHTML python script."

	def read(self, outputwriter):
		""" Start reading and parsing the XML-file. """

		import xml.sax.saxlib
		import xml.sax.saxexts

		class SaxHandler(xml.sax.saxlib.HandlerBase):
			""" The SaxHandler is our event-handler SAX calls on
			parsing the XML-file. """

			tablebase = ["html","body","table"]
			""" The table-base defines where we will find our table-tag
			that holds all the data we are interessted at. The default
			is to look at <html><body><table></table></body></html>. """
			
			def __init__(self, inputreader, outputwriter):
				""" Constructor. """

				# The to a SaxInput instance pointing inputreader.
				self.inputreader = inputreader
				# The to a KexiDBOutput instance pointing outputwriter.
				self.outputwriter = outputwriter
				# The hierachy-level in the DOM-tree we are in.
				self.level = 0
				# Defines if we are in the with tablebase defined DOM-element.
				self.intable = False

				# Points to a KexiDBOutput.Record instance if we are in a DOM-element that defines a record.
				self.record = None
				# Points to a KexiDBOutput.Field instance if we are in a record's field.
				self.field = None

			def startDocument(self):
				sys.stdout.write('=> Starting parsing\n')

			def endDocument(self):
				sys.stdout.write('=> Fineshed parsing\n')

			def startElement(self, name, attrs):
				""" This method is called by SAX if a DOM-element starts. """

				if self.level < len(self.tablebase):
					if self.tablebase[self.level] != name:
						self.intable = False
					else:
						self.intable = True
				self.level += 1
				if not self.intable:
					return

				# Print some debugging-output to stdout.
				for idx in range(self.level): sys.stdout.write('  ')
				sys.stdout.write('Element: %s' % name)
				for attrName in attrs.keys():
					sys.stdout.write(' %s="%s"' % (attrName,attrs.get(attrName)))
				sys.stdout.write('\n')

				# handle tr-, th- and td-tags inside the table.
				if name == "tr" and (self.level == len(self.tablebase) + 1):
					self.record = self.outputwriter.Record()
				elif name == "td" and (self.level == len(self.tablebase) + 2):
					self.field = self.outputwriter.Field()
				elif name == "th" and (self.level == len(self.tablebase) + 2):
					self.field = self.outputwriter.Field()

			def endElement(self, name):
				""" This method is called by SAX if a DOM-Element ends. """

				self.level -= 1
				#sys.stdout.write('EndElement:%s level:%s len(self.tablebase):%s\n' % (name,self.level,len(self.tablebase)))

				if self.record != None:
					# a record is defined. so, we are looking for the matching
					# end-tags to close a record or a field.
					if name == "tr" and (self.level == len(self.tablebase)):
						self.outputwriter.write(self.record)
						self.record = None
						self.field = None
					elif name == "td" and (self.level == len(self.tablebase) + 1):
						#if self.field == None:
						#	raise "Unexpected closing </td>"
						self.record.setField( self.field )
						self.field = None
					elif name == "th" and (self.level == len(self.tablebase) + 1):
						#if self.field == None:
						#	raise "Unexpected closing </td>"
						self.record.setHeader( self.field )
						self.field = None

			def characters(self, chars, offset, length):
				""" This method is called by SAX if the text-content of a DOM-Element
				was parsed. """

				if self.field != None:
					# the xml-data is unicode and we need to encode it
					# to latin-1 cause KexiDB deals only with latin-1.
					u = unicode(chars[offset:offset+length])
					self.field.append(u.encode("latin-1"))

		# start the job
		outputwriter.begin()
		# create saxhandler to handle parsing events.
		handler = SaxHandler(self, outputwriter)
		# we need a sax-parser and connect it with the handler.
		parser = xml.sax.saxexts.make_parser()
		parser.setDocumentHandler(handler)
		# open the XML-file, parse the content and close the file again.
		f = file(self.xmlfile, 'r')
		parser.parseFile(f)
		f.close()
		# job is done
		outputwriter.end()

class KexiDBOutput:
	""" The destination target we like to import the data to. This class
	provides abstract access to the KexiDB module. """

	class Result:
		""" Holds some informations about the import-result. """
		def __init__(self, outputwriter):
			self.outputwriter = outputwriter
			# number of records successfully imported.
			self.successcount = 0
			# number of records where import failed.
			self.failedcount = 0
			
		def addLog(self, record, state):
			import datetime
			date = datetime.datetime.now().strftime("%Y-%m-%d %H:%M.%S")
			self.outputwriter.logfile.write("%s (%s) %s\n" % (date,state,str(record)))

		def success(self, record):
			""" Called if a record was written successfully. """
			print "SUCCESS: %s" % str(record)
			self.successcount += 1
			if hasattr(self.outputwriter,"logfile"):
				self.addLog(record, "Success")

		def failed(self, record):
			""" Called if we failed to write a record. """
			print "FAILED: %s" % str(record)
			self.failedcount += 1
			if hasattr(self.outputwriter,"logfile"):
				self.addLog(record, "Failed")

	class Record:
		""" A Record in the dataset. """
		def __init__(self):
			self.fields = []
		def setHeader(self, headerfield):
			self.fields.append( headerfield )
			self.isHeader = True
		def setField(self, field):
			self.fields.append( field )
		def __str__(self):
			s = "["
			for f in self.fields:
				s += "%s, " % str(f)
			return s + "]"

	class Field:
		""" A field in a record. """
		def __init__(self):
			self.content = []
		def append(self, content):
			self.content.append( content )
		def __str__(self):
			return "".join(self.content)

	def __init__(self):
		""" Constructor. """
		import kexiapp
		keximainwindow = kexiapp.get("KexiAppMainWindow")
		
                try:
			self.connection = keximainwindow.getConnection()
		except:
			raise "No connection established. Please open a project before."

		self.fieldlist = None
		self.headerrecord = None
		self.mapping = {}

	def begin(self):
		""" Called before parsing starts. """
		print "START JOB"
		if self.fieldlist == None:
			raise "Invalid tableschema or fieldlist!"
		global KexiDBOutput
		self.result = KexiDBOutput.Result(self)
		if hasattr(self,"logfilename") and self.logfilename != None and self.logfilename != "":
			self.logfile = open(self.logfilename,'w')

	def end(self):
		""" Called if parsing is fineshed. """
		print "END JOB"
		self.logfile = None
		self.mapping = {}
		#self.headerrecord = None

	def getTables(self):
		""" return a list of avaiable tablenames. """
		tables = self.connection.tableNames()
		tables.sort()
		return tables

	def setTable(self, tablename):
		""" Set the tablename we like to import the data to. """
		tableschema = self.connection.tableSchema(tablename)
		if tableschema == None:
			raise "There exists no table with the name '%s'!" % tablename
		self.fieldlist = tableschema.fieldlist()
		fields = self.fieldlist.fields()
		for field in fields:
			print "KexiDBOutput.setTable(%s): %s(%s)" % (tablename,field.name(),field.type())
		print "names=%s" % self.fieldlist.names()
		
	def setMapping(self, mapping):
		""" Set the tablefieldname=xmlcolnr dictonary we should map the data to. """
		self.mapping = mapping

	def setLogFile(self, logfilename):
		""" Set the name of the logfile. """
		self.logfilename = logfilename

	def write(self, record):
		""" Write the record to the KexiDB table. """

		if hasattr(record, "isHeader"):
			self.headerrecord = record
			return

		sys.stdout.write('KexiDBOutput.write:')
		for f in record.fields:
			sys.stdout.write(' "%s"' % f)
		sys.stdout.write('\n')

		if hasattr(self,"onWrite"):
			if not self.onWrite(record):
				raise RuntimeError()
			delattr(self,"onWrite")
			self.fieldlist = self.fieldlist.subList( list( self.mapping ) )

		# Translate a KexiDBOutput.Record into a list of values.
		values = []
		for k in self.fieldlist.names():
			values.append( str(record.fields[ int(self.mapping[k]) ]) )
		print "Import values: %s" % values

		try:
			if self.connection.insertRecord(self.fieldlist, values):
				self.result.success(record)
			else:
				self.result.failed(record)
		except:
			err = self.connection.lastError()
			raise Exception( "Failed to insert the record:\n%s\n\n%s" % (values,err) )
			#raise Exception( "Failed to insert into table \"%s\" the record:\n%s\n%s" % (self.tableschema.name(),values,self.connection.lastError()) )

class GuiApp:
	""" The GUI-dialog displayed to let the user define the source
	XML-file and the destination KexiDB table. """

	class InitialDialog:
		def __init__(self, guiapp):
			self.guiapp = guiapp
			self.ok = False

			import gui
			self.dialog = gui.Dialog("Import XHTML")
			self.dialog.addLabel(self.dialog, "Import data from a XHTML-file to a KexiDB table.\n"
                                                  "The destination table needs to be an existing table the data should be added to.")
			self.importfile = self.dialog.addFileChooser(self.dialog,
				"Source File:",
				gui.getHome() + "/kexidata.xhtml",
				(('XHTML files', '*.xhtml'),('All files', '*')))

			self.desttable = self.dialog.addList(self.dialog, "Destination Table:", self.guiapp.outputwriter.getTables())

			#self.operation = self.dialog.addList(self.dialog, "Operation:", ("Insert","Update","Insert/Update"))
			#self.error = self.dialog.addList(self.dialog, "On error:", ("Ask","Skip","Abort"))

			self.logfile = self.dialog.addFileChooser(self.dialog,
				"Log File:",
				"",
				(('Logfiles', '*.log'),('All files', '*')))

			btnframe = self.dialog.addFrame(self.dialog)
			self.dialog.addButton(btnframe, "Next", self.doNext)
			self.dialog.addButton(btnframe, "Cancel", self.doCancel)
			self.dialog.show()

		def doCancel(self):
			""" Called if the Cancel-button was pressed. """
			self.dialog.close()
			self.dialog = None
			#self.guiapp.InitialDialog
			
		def doNext(self):
			""" Start to import the XML-file into the KexiDB table. """

			self.guiapp.inputreader.xmlfile = str(self.importfile.get())
			self.guiapp.outputwriter.setTable( str(self.desttable.get()) )
			self.guiapp.outputwriter.setLogFile( str(self.logfile.get()) )

			try:
				self.guiapp.inputreader.read( self.guiapp.outputwriter )
				
				msgbox = self.dialog.showMessageBox("info","Import done",
					"Successfully imported records: %s\nFailed to import records: %s" % (self.guiapp.outputwriter.result.successcount, self.guiapp.outputwriter.result.failedcount) )
				msgbox.show()
			
				self.doCancel()
			except RuntimeError, e:
				pass
			#except Exception, e:
			#	import traceback
			#	traceback.print_exc()
			#	msgbox = self.dialog.showMessageBox("error", "Error", e)
			#	msgbox.show()

	class MapperDialog:
		""" The dialog that provides us a way to map
		XHTML columns to the destination table. """

		def __init__(self, outputwriter, record):
			self.outputwriter = outputwriter
			self.ok = False
			fieldlist = outputwriter.fieldlist

			import gui
			self.dlg = gui.Dialog("Import XHTML")
			self.dlg.addLabel(self.dlg, "Define how the destination table should be mapped to the data from the XHTML file.")
			values = ["",]
			for i in range(len(record.fields)):
				try:
					values.append( "%s: %s" % (i,str(outputwriter.headerrecord.fields[i])) )
				except:
					values.append( "%s: (%s)" % (i,str(record.fields[i])) )

			self.items = []
			i = 0
			for field in fieldlist.fields():
				f = self.dlg.addFrame(self.dlg)

				l = self.dlg.addList(f, "%s:" % field.name(), values)
				self.items.append( (field,l) )

				details = "%s:" % str( field.type() )
				if field.isAutoInc(): details += "autoinc,"
				if field.isUniqueKey(): details += "unique,"
				if field.isNotNull(): details += "notnull,"
				if field.isNotEmpty(): details += "notempty,"
				self.dlg.addLabel(f, "(%s)" % details[:-1])

				try:
					variable = str( record.fields[i] )
					try:
						int(variable)
						i += 1
						if not field.isAutoInc():
							l.set(i)
					except ValueError, e:
						if not field.type() in ("Integer","BigInteger","ShortInteger","Float","Double"):
							i += 1
							l.set(i)
				except:
					pass

			btnframe = self.dlg.addFrame(self.dlg)
			self.dlg.addButton(btnframe, "Next", self.doNext)
			self.dlg.addButton(btnframe, "Cancel", self.dlg.close)
			self.dlg.show()

		def doNext(self):
			mapping = {}
			for item in self.items:
				(field,l) = item
				fieldname = field.name()
				colnr = str( l.get() ).split(":",1)[0]
				if colnr.isdigit():
					print "Table field '%s' is mapped to XML column '%s'" % (fieldname,colnr)
					mapping[ fieldname ] = colnr
			self.outputwriter.setMapping(mapping)
			self.ok = True
			self.dlg.close()

	def __init__(self, inputreader, outputwriter):
		""" Constructor. """

		self.inputreader = inputreader
		self.outputwriter = outputwriter
		self.outputwriter.onWrite = self.onWrite

		self.InitialDialog(self)

	def onWrite(self, record):
		""" This method got called after the first record got
		readed and before we start to import. """
		return self.MapperDialog(self.outputwriter, record).ok

GuiApp( SaxInput(), KexiDBOutput() )
