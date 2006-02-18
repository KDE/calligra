"""
CopyCenterPlugin to provide 'KexiDB'.

Description:
This python-script is a plugin for the CopyCenter.py.

Author:
Sebastian Sauer <mail@dipe.org>

Copyright:
GPL v2 or higher.
"""

class CopyCenterPlugin:
	""" The CopyCenterPlugin to provide abstract access to the 'KexiDB'
	framework to CopyCenter.py """

	name = "Kexi Database"
	""" The name this plugin has. The name should be unique and
	will be used for displaying a caption. """

	class Plugin:
		""" The implementation of a plugin which is published to the
		CopyCenter.py script. While there exists only one instance of
		the CopyCenterPlugin class, there will be n instances of this
		Plugin class (one for 'source' aka read-data-from and one for
		'destination' aka write-data-to) created from within the
		CopyCenter.py. The Source and Destination classes are extending
		this Plugin class with specialized functionality. """
		def __init__(self,copycenterplugin):
			""" Constructor. """
			self.copycenterplugin = copycenterplugin
			self.widget = None
			self.options = {
				'autoconnect' : False,
				'project' : '', #'~/test.kexi',
				'driver' : '', #'MySQL', #'SQLite3','MySQL',...
				'file' : '', #'/path/to/mysqlite3dbfile.kexi'
				'hostname' : '127.0.0.1',
				'port' : '',
				'usesocketfile' : False,
				'socketfile' : '',
				'username' : '',
				'password' : '',
				'database' : '',
				'table' : '',
				'fields' : '',
				'where' : '',
			}
			self.connection = copycenterplugin.Connection(self)
		def isFinished(self):
			return self.connection.isFinished()
		def finish(self):
			""" Called if reading is finished."""
			self.connection.finish()
		def createWidget(self, dialog, parent):
			""" Create and return a widget to modify the plugin settings. """
			return self.copycenterplugin.createWidget(dialog, self, parent)

	class Source(Plugin):
		""" Specialization of the Plugin class to implement the
		'source' aka read-data-from functionality. """
		plugintype = "Source"
		def init(self,copierer):
			""" Called if reading should be initialize. """
			self.connection.init(copierer)
			self.connection.initRead()
			# Called if a record should be readed.
			self.read = self.connection.readRecord

	class Destination(Plugin):
		""" Specialization of the Plugin class to implement the
		'destination' aka write-data-to functionality. """
		plugintype = "Destination"
		def init(self,copierer):
			""" Called if writing should be initialize. """
			self.connection.init(copierer)
			self.connection.initWrite()
			# Called if a record should be written.
			self.write = self.connection.writeRecord

	class Connection:
		""" Abstract access to work with KexiDB. """
		def __init__(self,plugin):
			self.plugin = plugin
			self.copierer = None
			self.kexidbconnection = None
			self.kexidbcursor = None
			self.tableschema = None
			self.fieldlist = None
		def lastError(self): return self.kexidbconnection.lastError()
		def connect(self): return self.kexidbconnection.connect()
		def disconnect(self):
			if self.kexidbconnection == None or self.kexidbconnection.disconnect():
				self.kexidbconnection = None
				return True
			return False
		def isConnected(self): return self.kexidbconnection != None and self.kexidbconnection.isConnected()
		def tableNames(self): return self.kexidbconnection.tableNames()
		def hasTableName(self,tablename): return tablename in self.kexidbconnection.tableNames()
		def tableSchema(self,tablename): return self.kexidbconnection.tableSchema(tablename)
		
		def init(self,copierer):
			self.copierer = copierer
			if self.kexidbconnection == None:
				if self.plugin.widget == None:
					raise "No connection established."
				self.copierer.appendProgressMessage("<i>Trying to connect...</i>")
				if self.plugin.widget.driverbox.driver == None:
					raise "Invalid driver."
				if not self.plugin.widget.connectClicked():
					raise "Failed to connect."
			connectiondata = self.kexidbconnection.data()
			self.copierer.appendProgressMessage("Connected: %s %s" % (connectiondata.driverName(),connectiondata.serverInfoString()))

			tablename = str(self.plugin.widget.tablebox.tableedit.text())
			if tablename == "":
				raise "No table defined"
			fields = [ f.strip() for f in str(self.plugin.widget.fieldbox.fieldsedit.text()).split(",") if len(f) > 0 ]
			if len(fields) < 1:
				raise "No fields defined"

			self.tableschema = self.kexidbconnection.tableSchema(tablename)
			if not self.tableschema: raise "No such tableschema \"%s\"" % tablename
			self.copierer.appendProgressMessage("Table: %s" % self.tableschema.name())

			if len(fields) == 1 and fields[0] == "*":
				self.fieldlist = self.tableschema.fieldlist()
			else:
				self.fieldlist = self.tableschema.fieldlist().subList(fields)
			if not self.fieldlist: raise "No such fields \"%s\"" % fields
			fieldlistnames = self.fieldlist.names()
			if len(fieldlistnames) < 1: raise "No valid fields defined for \"%s\"" % fields
			self.copierer.appendProgressMessage("Fields: %s" % fieldlistnames)
		def finish(self):
			if self.plugin.widget == None:
				self.disconnect()
			else:
				self.plugin.widget.disconnectClicked()
			self.kexidbcursor = None
			self.kexidbconnection = None
			self.tableschema = None
			self.fieldlist = None
			self.copierer = None
		def isFinished(self):
			return self.copierer == None

		def initRead(self):
			print "Initialize read"
			#queryschema = self.plugin.copycenterplugin.drivermanager.querySchema()
			queryschema = self.tableschema.query()
			queryschema.fieldlist().setFields(self.fieldlist)
			print "QuerySchema: %s" % queryschema.fieldlist().names()

			whereexpression = str(self.plugin.widget.whereedit.text())
			if whereexpression != "":
				print "WHERE-expression: %s" % whereexpression
				if not queryschema.setWhereExpression(whereexpression):
					raise "Invalid WHERE-expression."

			#print "QuerySchema statement=%s" % queryschema.statement()
			self.kexidbcursor = self.kexidbconnection.executeQuerySchema(queryschema)
			if not self.kexidbcursor:
				raise "Failed to create cursor."
			if not self.kexidbcursor.moveFirst():
				raise "The cursor has no records to read from."

		def readRecord(self):
			if self.kexidbcursor == None or self.kexidbcursor.eof():
				return None
			record = []
			for i in range( self.kexidbcursor.fieldCount() ):
				record.append( self.kexidbcursor.value(i) )
			self.kexidbcursor.moveNext()
			#print "read record: %s" % record
			return record

		def initWrite(self):
			print "Initialize write"

		def writeRecord(self,record):
			print "write record: %s" % record
			if self.kexidbconnection.insertRecord(self.fieldlist,record):
				print "=> insert successfully"
				self.copierer.writeSuccess(record, 1)
			else:
				print "=> insert failed: %s" % self.kexidbconnection.lastError()
  				self.copierer.writeFailed(record)
			#import time
  			#time.sleep(1)
  			return True

	def __init__(self, copycenter):
		""" Constructor.  """
		import krosskexidb
		self.drivermanager = krosskexidb.DriverManager()
		self.copycenter = copycenter

	def createWidget(self, dialog, plugin, parent):
		""" Each plugin may provide a qt.QWidget back to the
		CopyCenter.py. The widget will be used to configure our
		plugin settings. """

		import qt
		import os
		import re

		self.dialog = dialog
		self.mainbox = None
		class ProjectBox(qt.QHBox):
			def __init__(self,main,copycenterplugin,plugin,parent):
				self.main = main
				self.copycenterplugin = copycenterplugin
				self.plugin = plugin

				qt.QHBox.__init__(self,parent)
				prjlabel = qt.QLabel("Project File:",self)
				self.prjcombo = qt.QComboBox(self)
				self.prjcombo.setEditable(True)
				self.prjcombo.insertItem("")

				path = copycenterplugin.copycenter.homepath
				for f in os.listdir(path):
					file = os.path.join(path,f)
					if os.path.isfile(file) and re.search(".+\\.(kexi|kexis|kexic)$",f):
						self.prjcombo.insertItem(os.path.join("~",f))
						
				prjlabel.setBuddy(self.prjcombo)
				prjsavebtn = qt.QPushButton("...",self)
				qt.QObject.connect(prjsavebtn, qt.SIGNAL("clicked()"),self.buttonClicked)
				qt.QObject.connect(self.prjcombo, qt.SIGNAL("textChanged(const QString&)"), self.main.projectChanged)
				self.setStretchFactor(self.prjcombo,1)
			def buttonClicked(self):
				text = str(self.prjcombo.currentText())
				if text == "":
					text = self.copycenterplugin.copycenter.homepath
				elif re.search("^\\~(\\/|\\\\)",text):
					import os
					text = os.path.join(self.copycenterplugin.copycenter.homepath,text[2:])
				if self.plugin.plugintype == "Source":
					filename = qt.QFileDialog.getOpenFileName(text,"*.kexi *.kexis *.kexic;;*",self.copycenterplugin.dialog)
				else: # "Destination":
					filename = qt.QFileDialog.getSaveFileName(text,"*.kexi *.kexis *.kexic;;*",self.copycenterplugin.dialog)
				if str(filename) != "": self.prjcombo.setCurrentText(str(filename))

		class DriverBox(qt.QVBox):
			def __init__(self,main,parent):
				qt.QVBox.__init__(self,parent)
				self.main = main
				self.copycenterplugin = main.copycenterplugin
				self.plugin = main.plugin
				self.driver = None

				driverbox = qt.QHBox(self)
				driverlabel = qt.QLabel("Driver:",driverbox)
				self.drivercombo = qt.QComboBox(driverbox)
				self.drivercombo.insertItem("")
				for driver in self.copycenterplugin.drivermanager.driverNames():
					self.drivercombo.insertItem(driver)

				qt.QObject.connect(self.drivercombo, qt.SIGNAL("activated(int)"), self.activated)
				driverlabel.setBuddy(self.drivercombo)
				driverbox.setStretchFactor(self.drivercombo,1)
				
				self.box = qt.QVBox(self)
				self.mainbox = None

			def activated(self,index):
				drivertext = str(self.drivercombo.currentText())

				self.box.hide()
				if self.mainbox:
					self.mainbox.hide()
					self.mainbox.destroy()
					self.mainbox = None
				if index == 0 or drivertext == "":
					self.driver = None
					self.box.show()
					self.main.updateConnectButtons()
					return False

				self.driver = self.copycenterplugin.drivermanager.driver(drivertext)

				mainbox = qt.QVBox(self.box)
				mainbox.setSpacing(2)

				if self.driver.isFileDriver():
					filebox = qt.QHBox(mainbox)
					filelabel = qt.QLabel("File:",filebox)
					self.fileedit = qt.QLineEdit(self.plugin.options['file'],filebox)
					filelabel.setBuddy(self.fileedit)
					filebox.setStretchFactor(self.fileedit,1)
					filebtn = qt.QPushButton("...",filebox)
					qt.QObject.connect(filebtn, qt.SIGNAL("clicked()"), self.fileClicked)
				else:
					hostbox = qt.QHBox(mainbox)
					hostlabel = qt.QLabel("Hostname:",hostbox)
					self.hostedit = qt.QLineEdit(self.plugin.options['hostname'],hostbox)
					hostlabel.setBuddy(self.hostedit)
					hostbox.setStretchFactor(self.hostedit,1)

					portbox = qt.QHBox(mainbox)
					portlabel = qt.QLabel("Port:",portbox)
					self.portedit = qt.QLineEdit(self.plugin.options['port'],portbox)
					portlabel.setBuddy(self.portedit)
					portbox.setStretchFactor(self.portedit,1)

					sockbox = qt.QHBox(mainbox)
					self.sockfilecheckbox = qt.QCheckBox("Socket File:",sockbox)
					qt.QObject.connect(self.sockfilecheckbox, qt.SIGNAL("toggled(bool)"), self.sockfilecheckboxClicked)
					self.sockfilebox = qt.QHBox(sockbox)
					self.sockfileedit = qt.QLineEdit(self.plugin.options['socketfile'],self.sockfilebox)
					self.sockfilebox.setEnabled(False)
					sockfilebtn = qt.QPushButton("...",self.sockfilebox)
					self.sockfilecheckbox.setChecked( str(self.plugin.options['usesocketfile']) == str(True) )
					qt.QObject.connect(sockfilebtn, qt.SIGNAL("clicked()"), self.sockfileClicked)
					self.sockfilebox.setStretchFactor(self.sockfileedit,1)
					sockbox.setStretchFactor(self.sockfilebox,1)

					userbox = qt.QHBox(mainbox)
					userlabel = qt.QLabel("Username:",userbox)
					self.useredit = qt.QLineEdit(self.plugin.options['username'],userbox)
					userlabel.setBuddy(self.useredit)
					userbox.setStretchFactor(self.useredit,1)

					passbox = qt.QHBox(mainbox)
					passlabel = qt.QLabel("Password:",passbox)
					self.passedit = qt.QLineEdit(self.plugin.options['password'],passbox)
					self.passedit.setEchoMode(qt.QLineEdit.Password)
					passlabel.setBuddy(self.passedit)
					passbox.setStretchFactor(self.passedit,1)

					dbbox = qt.QHBox(mainbox)
					dblabel = qt.QLabel("Database:",dbbox)
					self.dbedit = qt.QLineEdit(self.plugin.options['database'],dbbox)
					dblabel.setBuddy(self.dbedit)
					dbbox.setStretchFactor(self.dbedit,1)
				#self.tablecombo.setText("")

				self.mainbox = mainbox
				self.mainbox.show()
				self.box.show()
				self.main.updateConnectButtons()
				return True

			def fileClicked(self):
				text = str(self.fileedit.text())
				if text == "": text = self.copycenterplugin.copycenter.homepath
				if self.plugin.plugintype == "Source":
					filename = qt.QFileDialog.getOpenFileName(text,"*",self.copycenterplugin.dialog)
				else: # "Destination":
					filename = qt.QFileDialog.getSaveFileName(text,"*",self.copycenterplugin.dialog)
				if str(filename) != "": self.fileedit.setText(str(filename))
			def sockfilecheckboxClicked(self,checked):
				self.sockfilebox.setEnabled(checked)
				
			def sockfileClicked(self):
				text = str(self.sockfileedit.text())
				if text == "": text = self.copycenterplugin.copycenter.homepath
				if self.plugin.plugintype == "Source":
					filename = qt.QFileDialog.getOpenFileName(text,"*",self.copycenterplugin.dialog)
				else: # "Destination":
					filename = qt.QFileDialog.getSaveFileName(text,"*",self.copycenterplugin.dialog)
				if str(filename) != "": self.sockfileedit.setText(str(filename))

		class TableBox(qt.QHBox):
			def __init__(self,copycenterplugin,plugin,parent):
				qt.QHBox.__init__(self,parent)
				self.copycenterplugin = copycenterplugin
				self.plugin = plugin
				tablelabel = qt.QLabel("Table:",self)
				self.tableedit = qt.QLineEdit(self.plugin.options['table'],self)
				self.tablebtn = qt.QPushButton("...",self)
				self.tablebtn.setEnabled(False)
				qt.QObject.connect(self.tablebtn, qt.SIGNAL("clicked()"), self.buttonClicked)
				tablelabel.setBuddy(self.tableedit)
				self.setStretchFactor(self.tableedit,1)
			def buttonClicked(self):
				ListViewDialog = self.copycenterplugin.dialog.ListViewDialog
				class TableDialog(ListViewDialog):
					def __init__(self,tablebox):
						ListViewDialog.__init__(self,tablebox,"Tables")
						self.mainwidget = tablebox
						self.listview.addColumn("Name")
						text = str(self.mainwidget.tableedit.text())
						item = None
						for table in self.mainwidget.plugin.connection.tableNames():
							if item == None:
								item = qt.QListViewItem(self.listview,table)
							else:
								item = qt.QListViewItem(self.listview,item,table)
							if table == text:
								self.listview.setSelected(item,True)
								self.listview.ensureItemVisible(item)
						qt.QObject.connect(self.listview, qt.SIGNAL("doubleClicked(QListViewItem*, const QPoint&, int)"), self.okClicked)
						qt.QObject.connect(self.okbtn, qt.SIGNAL("clicked()"), self.okClicked)
					def okClicked(self):
						item = self.listview.selectedItem()
						if item == None:
							self.mainwidget.tableedit.setText("")
						else:
							self.mainwidget.tableedit.setText(item.text(0))
						self.close()
				dialog = TableDialog(self)
				dialog.show()

		class FieldBox(qt.QHBox):
			def __init__(self,copycenterplugin,plugin,parent):
				qt.QHBox.__init__(self,parent)
				self.copycenterplugin = copycenterplugin
				self.plugin = plugin
				self.tablename = ""
				fieldslabel = qt.QLabel("Fields:",self)
				self.fieldsedit = qt.QLineEdit(self.plugin.options['fields'],self)
				self.setStretchFactor(self.fieldsedit,1)
				fieldslabel.setBuddy(self.fieldsedit)
				self.fieldsbtn = qt.QPushButton("...",self)
				self.fieldsbtn.setEnabled(False)
				qt.QObject.connect(self.fieldsbtn, qt.SIGNAL("clicked()"), self.fieldsClicked)
			def fieldsClicked(self):
				ListViewDialog = self.copycenterplugin.dialog.ListViewDialog
				class FieldsDialog(ListViewDialog):
					def __init__(self, fieldbox):
						ListViewDialog.__init__(self,fieldbox,"Fields")
						self.fieldbox = fieldbox
						self.listview.setSelectionMode(qt.QListView.Multi)
						self.listview.setSorting(-1)
						self.listview.header().setClickEnabled(False)
						self.listview.addColumn("Name")
						self.listview.addColumn("Type")
						self.listview.addColumn("Options")
						fieldslist = str(self.fieldbox.fieldsedit.text()).split(",")
						allfields = ("*" in fieldslist)
						tableschema = self.fieldbox.plugin.connection.tableSchema(self.fieldbox.tablename)
						item = None
						for field in tableschema.fieldlist().fields():
							opts = []
							for opt in ("isAutoInc","isNotNull","isNotEmpty"):
								if getattr(field,opt)():
									opts.append(opt[2:])
							item = self.addItem(( field.name(),field.type(),",".join(opts) ),item)
							if allfields or field.name() in fieldslist:
								self.listview.setSelected(item,True)
						qt.QObject.connect(self.okbtn, qt.SIGNAL("clicked()"), self.okClicked)
					def okClicked(self):
						selitems = []
						item = self.listview.firstChild()
						while item:
							if item.isSelected():
								selitems.append(str(item.text(0)))
							item = item.nextSibling()
						self.fieldbox.fieldsedit.setText(",".join(selitems))
						self.close()
				dialog = FieldsDialog(self)
				dialog.show()
			def tableChanged(self, text):
				self.tablename = str(text)
				if self.plugin.connection.isConnected():
					if self.plugin.connection.hasTableName(self.tablename):
						self.fieldsbtn.setEnabled(True)
						return
				self.fieldsbtn.setEnabled(False)

		class MainBox(qt.QHBox):
			def __init__(self,copycenterplugin,plugin,parent):
				qt.QHBox.__init__(self,parent)
				self.copycenterplugin = copycenterplugin
				self.plugin = plugin

				self.prjbox = ProjectBox(self,copycenterplugin,plugin,parent)
				self.driverbox = DriverBox(self,parent)

				statusbar = qt.QHBox(parent)
				statusbar.setSpacing(2)
				#self.statuslabel = qt.QLabel("Disconnected",statusbar)
				#statusbar.setStretchFactor(self.statuslabel,1)
				statusbar.setStretchFactor(qt.QWidget(statusbar),1)
				self.connectbtn = qt.QPushButton("Connect",statusbar)
				self.connectbtn.setEnabled(False)
				qt.QObject.connect(self.connectbtn, qt.SIGNAL("clicked()"),self.connectClicked)
				self.disconnectbtn = qt.QPushButton("Disconnect",statusbar)
				self.disconnectbtn.setEnabled(False)
				qt.QObject.connect(self.disconnectbtn, qt.SIGNAL("clicked()"),self.disconnectClicked)

				#self.connectionbox = ConnectionBox(copycenterplugin,plugin,parent)
				self.tablebox = TableBox(copycenterplugin,plugin,parent)
				self.fieldbox = FieldBox(copycenterplugin,plugin,parent)
				qt.QObject.connect(self.tablebox.tableedit, qt.SIGNAL("textChanged(const QString&)"), self.fieldbox.tableChanged)

				if self.plugin.options['project'] != '':
					self.prjbox.prjcombo.setCurrentText(self.plugin.options['project'])

				if self.plugin.options['driver'] != '':
					try:
						item = str(self.driverbox.drivercombo.listBox().findItem(self.plugin.options['driver'],qt.Qt.ExactMatch).text())
						self.driverbox.drivercombo.setCurrentText(item)
						self.driverbox.activated(item)
					except:
						pass

				if self.plugin.plugintype == "Destination":
					#typebox = qt.QHBox(parent)
					#label = qt.QLabel("Operation:",typebox)
					#combobox = qt.QComboBox(typebox)
					#combobox.insertItem("Append")
					#combobox.insertItem("Replace")
					#combobox.insertItem("Update")
					#combobox.insertItem("Update/Insert")
					#combobox.insertItem("Insert new")
					#label.setBuddy(combobox)
					#typebox.setStretchFactor(combobox,1)
					pass
				elif self.plugin.plugintype == "Source":
					wherebox = qt.QHBox(parent)
					wherelabel = qt.QLabel("Where:",wherebox)
					self.whereedit = qt.QLineEdit(self.plugin.options['where'],wherebox)

					#orderbox = qt.QHBox(parent)
					#orderlabel = qt.QLabel("Order By:",orderbox)
					#orderedit = qt.QLineEdit("",orderbox)

				#errbox = qt.QHBox(parent)
				#errlabel = qt.QLabel("On Error:",errbox)
				#errcombo = qt.QComboBox(errbox)
				#errcombo.insertItem("Ask")
				#errcombo.insertItem("Skip")
				#errcombo.insertItem("Abort")
				#errlabel.setBuddy(errcombo)
				#errbox.setStretchFactor(errcombo,1)

				if self.plugin.options['autoconnect']:
					self.connectClicked()

			def projectChanged(self, text):
				#if self.driverbox.drivercombo.currentItem() != 0:
				#	self.driverbox.drivercombo.setCurrentItem(0)

				file = str(text)
				import os
				if re.search("^\\~(\\/|\\\\)",file):
					file = os.path.join(self.copycenterplugin.copycenter.homepath,file[2:])
				if file == "" or not os.path.isfile(file):
					self.driverbox.drivercombo.setCurrentItem(0)
					self.driverbox.activated(0)
					return

				connectiondata = self.copycenterplugin.drivermanager.createConnectionDataByFile(file)
				if connectiondata == None:
					raise "Unsupported file."

				drivername = connectiondata.driverName().lower()
				print "driver: %s" % drivername
				for i in range(1,self.driverbox.drivercombo.count()):
					if drivername == self.driverbox.drivercombo.text(i).lower():
						self.driverbox.drivercombo.setCurrentItem(i)
						self.driverbox.activated(i)
						break

				if self.driverbox.driver != None:
					if self.driverbox.driver.isFileDriver():
						self.driverbox.fileedit.setText(connectiondata.fileName())
					else: # server
						self.driverbox.hostedit.setText(connectiondata.hostName())
						self.driverbox.portedit.setText(str(connectiondata.port()))
						self.driverbox.sockfilecheckbox.setChecked(connectiondata.localSocketFileUsed())
						self.driverbox.sockfileedit.setText(connectiondata.localSocketFileName())
						self.driverbox.useredit.setText(connectiondata.userName())
						self.driverbox.passedit.setText(connectiondata.password())
						self.driverbox.dbedit.setText(connectiondata.databaseName())

			def connectClicked(self):
				if self.driverbox.driver == None:
					print "No driver selected."
					return False
				connectiondata = self.copycenterplugin.drivermanager.createConnectionData()
				if self.driverbox.driver.isFileDriver():
					file = str(self.driverbox.fileedit.text())
					if file == "" or not os.path.isfile(file):
						qt.QMessageBox.critical(self,"Failed to connect","There exists no such database file \"%s\"" % file)
						return False
					connectiondata.setFileName(file)
					connectiondata.setDatabaseName(file)
				else:
					connectiondata.setHostName(str(self.driverbox.hostedit.text()))
					connectiondata.setPort(str(self.driverbox.portedit.text()))
					connectiondata.setLocalSocketFileUsed(self.driverbox.sockfilecheckbox.isChecked())
					connectiondata.setLocalSocketFileName(str(self.driverbox.sockfileedit.text()))
					connectiondata.setPassword(str(self.driverbox.passedit.text()))
					connectiondata.setUserName(str(self.driverbox.useredit.text()))
					connectiondata.setDatabaseName(str(self.driverbox.dbedit.text()))
				print "Creating connection"
				connection = self.driverbox.driver.createConnection(connectiondata)
				print "Trying to connect"
				if not connection.connect():
					qt.QMessageBox.critical(self,"Failed to connect",connection.lastError())
					return False
				print "Use database \"%s\"" % connectiondata.databaseName()
				if not connection.useDatabase( connectiondata.databaseName() ):
					qt.QMessageBox.critical(self,"Failed to connect",connection.lastError())
					return False
				print "dbnames = %s" % connection.databaseNames()
				print "tablenames = %s" % connection.tableNames()
				#self.useDatabase(connection, filename)

				self.plugin.connection.kexidbconnection = connection
				self.updateConnectButtons()
				return True

			def disconnectClicked(self):
				if not self.plugin.connection.disconnect():
					qt.QMessageBox.critical(self,"Failed to disconnect",self.plugin.connection.lastError())
					return
				self.updateConnectButtons()

			def updateConnectButtons(self):
				connected = self.plugin.connection.isConnected()
				self.prjbox.setEnabled(not connected)
				self.driverbox.setEnabled(not connected)
				self.connectbtn.setEnabled( (not connected) and (self.driverbox.driver != None) )
				self.disconnectbtn.setEnabled(connected)
				self.tablebox.tablebtn.setEnabled(connected)
				self.fieldbox.tableChanged(self.tablebox.tableedit.text())

			def getOptionValue(self,optionname):
				try:
					if optionname == 'project': return str(self.prjbox.prjcombo.currentText())
					elif optionname == 'driver': return str(self.driverbox.drivercombo.currentText())
					elif optionname == 'file': return str(self.driverbox.fileedit.text())
					elif optionname == 'hostname': return str(self.driverbox.hostedit.text())
					elif optionname == 'port': return str(self.driverbox.portedit.text())
					elif optionname == 'usesocketfile': return str(self.driverbox.sockfilecheckbox.isChecked())
					elif optionname == 'socketfile': return str(self.driverbox.sockfileedit.text())
					elif optionname == 'username': return str(self.driverbox.useredit.text())
					elif optionname == 'password': return str(self.driverbox.passedit.text())
					elif optionname == 'database': return str(self.driverbox.dbedit.text())
					elif optionname == 'table': return str(self.tablebox.tableedit.text())
					elif optionname == 'fields': return str(self.fieldbox.fieldsedit.text())
					elif optionname == 'where': return str(self.whereedit.text())
				except:
					pass
				return ""
				
		mainbox = MainBox(self,plugin,parent)
		plugin.widget = mainbox
		return mainbox

