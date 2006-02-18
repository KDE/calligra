"""
CopyCenterPlugin to provide 'QtSQL'.

Description:
This python-script is a plugin for the CopyCenter.py.

Author:
Sebastian Sauer <mail@dipe.org>

Copyright:
GPL v2 or higher.
"""

class CopyCenterPlugin:
	""" The CopyCenterPlugin to provide 'QtSQL' to CopyCenter.py """

	name = "QtSQL Database"
	""" The name this plugin has. The name should be unique and
	will be used for displaying a caption. """
	
	class Plugin:
		def _init_(self,copycenterplugin):
			self.copycenterplugin = copycenterplugin
			self.widget = None
			self.database = None
			self.cursor = None
			self.isfinished = True
		def _init(self,copierer):
			self.copierer = copierer
			if not self.widget.connectClicked():
				raise "Failed to connect with database."
			if self.database == None or not self.database.isOpen():
				raise "Database is not initialized or not opened."
			self.copierer.appendProgressMessage("Connected: %s %s@%s:%i %s" %
				(str(self.database.driverName()),str(self.database.userName()),str(self.database.hostName()),self.database.port(),str(self.database.databaseName())) )
			self.isfinished = False
		def isFinished(self):
			return self.isfinished
		def finish(self):
			self.isfinished = True
			self.widget.disconnectClicked()
		def createWidget(self,dialog,parent):
			return self.copycenterplugin.widget(dialog, self, parent)

	class Source(Plugin):
		plugintype = "Source"
		def __init__(self,copycenterplugin):
			self._init_(copycenterplugin)
			self.options = {
				'driver': 'QMYSQL3', #'QMYSQL3','QPSQL7','QODBC3',...
				'hostname': '127.0.0.1',
				'port': 3306,
				'username': 'root', #'MyUsername',
				'password': '', #'MySecretPassword',
				'database': '', #'MyQtSQLDatabase',
				'table': '', #'table1',
				'fields': '', #'f1,f2',
				'where': '',
			}
		def init(self,copierer):
			self._init(copierer)
			tablename = str(self.widget.tableedit.text())
			wherestatement = str(self.widget.whereedit.text())
			import qt
			import qtsql
			self.cursor = qtsql.QSqlCursor(tablename,True,self.database)
			self.cursor.setFilter(wherestatement)
			if not self.cursor.select():
				raise "Select on cursor failed.<br>%s<br>%s" % ( str(self.cursor.lastError().driverText()),str(self.cursor.lastError().databaseText()) )
			self.fieldlist = []
			for fieldname in str(self.widget.fieldedit.text()).split(","):
				fn = fieldname.strip()
				if fn != "":
					field = self.cursor.field(fn)
					if not field:
						raise "There exists no such field \"%s\" in the table \"%s\"." % (fn,tablename)
					self.fieldlist.append(str(field.name()))
			if len(self.fieldlist) < 1:
				raise "No fields for table \"%s\" defined." % tablename
			copierer.appendProgressMessage("SQL: %s" % str(self.cursor.executedQuery()))

		def read(self):
			if not self.cursor.next():
				return None
			record = []
			for fieldname in self.fieldlist:
				record.append( unicode(self.cursor.value(fieldname).toString()).encode("latin-1") )
			#print "read record: %s" % record
			return record

	class Destination(Plugin):
		plugintype = "Destination"
		def __init__(self,copycenterplugin):
			self._init_(copycenterplugin)
			self.options = {
				'driver': 'QMYSQL3', #'QMYSQL3','QPSQL7','QODBC3',...
				'hostname': '127.0.0.1',
				'port': 3306,
				'username': 'root', #'MyUsername',
				'password': '', #'MySecretPassword',
				'database': '', #'MyQtSQLDatabase',
				'table': '', #'table2',
				'fields': '', #'field1,field2',
				'operation': 'Insert', #'Insert','Update'...
				'indexfield': '',
			}
		def init(self,copierer):
			self._init(copierer)
			import qt
			import qtsql

			self.fieldlist = []
			for fieldname in str(self.widget.fieldedit.text()).split(","):
				fn = fieldname.strip()
				if fn != "": self.fieldlist.append(fn)

			tablename = str(self.widget.tableedit.text())
			self.cursor = qtsql.QSqlCursor(tablename,True,self.database)
			{
				0: self.initInsert,
				1: self.initUpdate
			}[ self.widget.operationedit.currentItem() ]()

		def initInsert(self):
			self.write = self.writeInsert
			if not self.cursor.select():
				raise "Select on cursor failed.<br>%s<br>%s" % ( str(self.cursor.lastError().driverText()),str(self.cursor.lastError().databaseText()) )
			for fieldname in self.fieldlist: # check fieldlist
				field = self.cursor.field(fieldname)
				if not field: raise "There exists no such field \"%s\" in the table \"%s\"." % (fieldname, self.cursor.name())
			self.copierer.appendProgressMessage("Insert SQL: %s" % str(self.cursor.executedQuery()))

		def writeInsert(self, record):
			print "insert record: %s" % record
			import qt
			cursorrecord = self.cursor.primeInsert()
			count = len(record)
			for i in range(len(self.fieldlist)):
				if i == count: break
				r = record[i]
				if r == None:
					v = qt.QVariant()
				else:
					v = qt.QVariant(r)
				cursorrecord.setValue(self.fieldlist[i], v)
			rowcount = self.cursor.insert()
			if rowcount < 1:
				drv = unicode(self.cursor.lastError().driverText()).encode("latin-1")
				db = unicode(self.cursor.lastError().databaseText()).encode("latin-1")
				print "failed: %s %s" % (drv,db)
				self.copierer.writeFailed(record)
			else:
				self.copierer.writeSuccess(record,rowcount)
			#import time
			#time.sleep(1)
			return True

		def initUpdate(self):
			self.write = self.writeUpdate
			self.indexfieldname = str(self.widget.indexedit.text()).strip()
			if self.indexfieldname == "": raise "No index-field defined."
			pkindex = self.cursor.index(self.indexfieldname)
			if not pkindex: raise "Invalid index-field defined."
			self.cursor.setPrimaryIndex(pkindex)
			#self.cursor.setMode( qtsql.QSqlCursor.Insert | qtsql.QSqlCursor.Update )
			self.copierer.appendProgressMessage("Update SQL: %s" % str(self.cursor.executedQuery()))

		def writeUpdate(self, record):
			import qt
			# determinate the primary-index
			try:
				idx = self.fieldlist.index(self.indexfieldname)
				indexvalue = record[idx]
			except:
				import traceback
				print "".join( traceback.format_exception(sys.exc_info()[0],sys.exc_info()[1],sys.exc_info()[2]) )
				raise "Failed to determinate the value for the primary key."
			# select cursor and go to matching record.
			wherestatement = "%s = \"%s\"" % (self.indexfieldname, indexvalue)
			if not self.cursor.select(wherestatement):
				raise "Select on cursor failed.<br>%s<br>%s" % ( str(self.cursor.lastError().driverText()),str(self.cursor.lastError().databaseText()) )
			if not self.cursor.next():
				#print "No such record to update !"
				return False
			# Prepare updating the record.
			cursorrecord = self.cursor.primeUpdate()
			# Update the fields in the record.
			count = len(record)
			for i in range(len(self.fieldlist)):
				if i == count: break
				fieldname = self.fieldlist[i]
				if self.indexfieldname != fieldname: # don't update the indexfield!
					r = record[i]
					if r == None:
						v = qt.QVariant()
					else:
						v = qt.QVariant(r)
					cursorrecord.setValue(fieldname, v)
			# Write updated record.
			rowcount = self.cursor.update()
			if rowcount < 1:
				self.copierer.writeFailed(record)
			else:
				self.copierer.writeSuccess(record,rowcount)
				print "updated record (rowcount %s): %s" % (rowcount,record)
			return True

	def __init__(self, copycenter):
		""" Constructor. """
		pass

	def widget(self,dialog,plugin,parent):
		""" Each plugin may provide a qt.QWidget back to the
		CopyCenter.py. The widget will be used to configure our
		plugin settings. """

		import qt
		import os

		self.dialog = dialog
		ListViewDialog = self.dialog.ListViewDialog
		class TableDialog(ListViewDialog):
			def __init__(self, mainwidget):
				ListViewDialog.__init__(self,mainwidget,"Tables")
				self.mainwidget = mainwidget
				self.listview.addColumn("Name")
				text = str(self.mainwidget.tableedit.text())
				item = None
				for table in self.mainwidget.plugin.database.tables():
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

		class FieldsDialog(ListViewDialog):
			def __init__(self, mainwidget):
				ListViewDialog.__init__(self,parent,"Fields")
				self.mainwidget = mainwidget
				self.listview.setSelectionMode(qt.QListView.Multi)
				self.listview.setSorting(-1)
				self.listview.header().setClickEnabled(False)
				self.listview.addColumn("Name")
				self.listview.addColumn("Type")
				self.listview.addColumn("Options")
				tablename = str(self.mainwidget.tableedit.text())
				recinfo = self.mainwidget.plugin.database.recordInfo(tablename)
				if recinfo != None:
					fieldslist = str(self.mainwidget.fieldedit.text()).split(",")
					allfields = ("*" in fieldslist)
					item = None
					for fieldinfo in recinfo:
						opts = ""
						for s in ('Required','Calculated'): #,'Generated'):
							if getattr(fieldinfo,"is%s" % s)(): opts += "%s " % s
						item = self.addItem((fieldinfo.name(), qt.QVariant.typeToName(fieldinfo.type()), opts),item)
						if allfields or fieldinfo.name() in fieldslist:
							self.listview.setSelected(item,True)
				qt.QObject.connect(self.okbtn, qt.SIGNAL("clicked()"), self.okClicked)
			def okClicked(self):
				selitems = []
				item = self.listview.firstChild()
				while item:
					if item.isSelected():
						selitems.append(str(item.text(0)))
					item = item.nextSibling()
				self.mainwidget.fieldedit.setText(",".join(selitems))
				self.close()


		class MainWidget(qt.QHBox):
			def __init__(self,plugin,dialog,parent):
				import qt
				import qtsql
				qt.QHBox.__init__(self,parent)
				self.dialog = dialog
				self.plugin = plugin

				self.connectionbox = qt.QVBox(parent)
				self.connectionbox.setSpacing(2)

				driverbox = qt.QHBox(self.connectionbox)
				driverlabel = qt.QLabel("Driver:",driverbox)
				self.driveredit = qt.QComboBox(driverbox)
				for driver in qtsql.QSqlDatabase.drivers():
					self.driveredit.insertItem(driver)
					if self.plugin.options['driver'] == driver:
						self.driveredit.setCurrentItem(self.driveredit.count() - 1)
				driverlabel.setBuddy(self.driveredit)
				driverbox.setStretchFactor(self.driveredit,1)

				hostbox = qt.QHBox(self.connectionbox)
				hostlabel = qt.QLabel("Hostname:",hostbox)
				self.hostedit = qt.QLineEdit(self.plugin.options['hostname'],hostbox)
				hostlabel.setBuddy(self.hostedit)
				hostbox.setStretchFactor(self.hostedit,1)
				
				portbox = qt.QHBox(self.connectionbox)
				portlabel = qt.QLabel("Port:",portbox)
				self.portedit = qt.QLineEdit(str(self.plugin.options['port']),portbox)
				portlabel.setBuddy(self.portedit)
				portbox.setStretchFactor(self.portedit,1)

				userbox = qt.QHBox(self.connectionbox)
				userlabel = qt.QLabel("Username:",userbox)
				self.useredit = qt.QLineEdit(self.plugin.options['username'],userbox)
				userlabel.setBuddy(self.useredit)
				userbox.setStretchFactor(self.useredit,1)
				
				passbox = qt.QHBox(self.connectionbox)
				passlabel = qt.QLabel("Password:",passbox)
				self.passedit = qt.QLineEdit(self.plugin.options['password'],passbox)
				self.passedit.setEchoMode(qt.QLineEdit.Password)
				passlabel.setBuddy(self.passedit)
				passbox.setStretchFactor(self.passedit,1)

				dbbox = qt.QHBox(self.connectionbox)
				dblabel = qt.QLabel("Database:",dbbox)
				self.dbedit = qt.QLineEdit(self.plugin.options['database'],dbbox)
				dblabel.setBuddy(self.dbedit)
				dbbox.setStretchFactor(self.dbedit,1)
				
				statusbar = qt.QHBox(parent)
				statusbar.setSpacing(2)
				statusbar.setStretchFactor(qt.QWidget(statusbar),1)
				self.connectbtn = qt.QPushButton("Connect",statusbar)
				qt.QObject.connect(self.connectbtn, qt.SIGNAL("clicked()"),self.connectClicked)
				self.disconnectbtn = qt.QPushButton("Disconnect",statusbar)
				self.disconnectbtn.setEnabled(False)
				qt.QObject.connect(self.disconnectbtn, qt.SIGNAL("clicked()"),self.disconnectClicked)

				tablebox = qt.QHBox(parent)
				tablelabel = qt.QLabel("Table:",tablebox)
				self.tableedit = qt.QLineEdit(self.plugin.options['table'],tablebox)
				qt.QObject.connect(self.tableedit, qt.SIGNAL("textChanged(const QString&)"), self.tableEditChanged)
				self.tablebtn = qt.QPushButton("...",tablebox)
				self.tablebtn.setEnabled(False)
				qt.QObject.connect(self.tablebtn, qt.SIGNAL("clicked()"), self.tableBtnClicked)
				tablelabel.setBuddy(self.tableedit)
				tablebox.setStretchFactor(self.tableedit,1)

				fieldbox = qt.QHBox(parent)
				fieldlabel = qt.QLabel("Fields:",fieldbox)
				self.fieldedit = qt.QLineEdit(self.plugin.options['fields'],fieldbox)
				self.fieldbtn = qt.QPushButton("...",fieldbox)
				self.fieldbtn.setEnabled(False)
				qt.QObject.connect(self.fieldbtn, qt.SIGNAL("clicked()"), self.fieldBtnClicked)
				fieldlabel.setBuddy(self.fieldedit)
				fieldbox.setStretchFactor(self.fieldedit,1)

				if self.plugin.plugintype == "Source":
					box = qt.QHBox(parent)
					wherelabel = qt.QLabel("Where:",box)
					self.whereedit = qt.QLineEdit(self.plugin.options['where'],box)
					wherelabel.setBuddy(self.whereedit)
					box.setStretchFactor(self.whereedit,1)
				elif self.plugin.plugintype == "Destination":
					
					class OperationBox(qt.QVBox):
						def __init__(self, mainwidget, parent):
							self.mainwidget = mainwidget
							qt.QVBox.__init__(self, parent)
							opbox = qt.QHBox(self)
							operationlabel = qt.QLabel("Operation:",opbox)
							self.mainwidget.operationedit = qt.QComboBox(opbox)
							for op in ('Insert','Update'):
								self.mainwidget.operationedit.insertItem(op)
								if self.mainwidget.plugin.options['operation'] == op:
									self.mainwidget.operationedit.setCurrentItem(self.mainwidget.operationedit.count() - 1)
							operationlabel.setBuddy(self.mainwidget.operationedit)
							opbox.setStretchFactor(self.mainwidget.operationedit,1)
							self.box = None
							qt.QObject.connect(self.mainwidget.operationedit, qt.SIGNAL("activated(int)"), self.operationActivated)
							self.operationActivated()
						def operationActivated(self, **args):
							if self.box:
								self.box.hide()
								self.box.destroy()
								self.box = None
							def showInsert(self):
								pass
							def showUpdate(self):
								self.box = qt.QHBox(self)
								indexlabel = qt.QLabel("Indexfield:", self.box)
								self.mainwidget.indexedit = qt.QLineEdit(self.mainwidget.plugin.options['indexfield'], self.box)
								indexlabel.setBuddy(self.mainwidget.indexedit)
								self.box.setStretchFactor(self.mainwidget.indexedit,1)
							{
								0: showInsert,
								1: showUpdate,
							}[ self.mainwidget.operationedit.currentItem() ](self)
							if self.box != None: self.box.show()
					OperationBox(self,parent)

			def tableEditChanged(self,text):
				if self.plugin.database != None and self.plugin.database.isOpen():
					if str(text) in self.plugin.database.tables():
						self.fieldbtn.setEnabled(True)
						return
				self.fieldbtn.setEnabled(False)

			def tableBtnClicked(self):
				dialog = TableDialog(self)
				dialog.show()

			def fieldBtnClicked(self):
				dialog = FieldsDialog(self)
				dialog.show()

			def updateConnectState(self):
				connected = self.plugin.database != None and self.plugin.database.isOpen()
				self.connectionbox.setEnabled(not connected)
				self.connectbtn.setEnabled(not connected)
				self.disconnectbtn.setEnabled(connected)
				self.tablebtn.setEnabled(connected)
				self.tableEditChanged(self.tableedit.text())

			def getOptionValue(self,optionname):
				try:
					if optionname == 'driver': return str(self.driveredit.currentText())
					if optionname == 'hostname': return str(self.hostedit.text())
					if optionname == 'port': return str(self.portedit.text())
					if optionname == 'username': return str(self.useredit.text())
					if optionname == 'password': return str(self.passedit.text())
					if optionname == 'database': return str(self.dbedit.text())
					if optionname == 'table': return str(self.tableedit.text())
					if optionname == 'fields': return str(self.fieldedit.text())
					if optionname == 'where': return str(self.whereedit.text())
					if optionname == 'operation': return str(self.operationedit.currentText())
					if optionname == 'indexfield': return str(self.indexedit.text())
				except:
					import traceback
					print "".join( traceback.format_exception(sys.exc_info()[0],sys.exc_info()[1],sys.exc_info()[2]) )
				return ""

			def connectClicked(self):
				if self.plugin.database != None and self.plugin.database.isOpen():
					print "already connected. not needed to reconnect..."
					self.updateConnectState()
					return True
				print "trying to connect..."

				import qtsql
				drivername = str(self.driveredit.currentText())
				print "drivername: %s" % drivername
				connectionname = "CopyCenter%s" % self.plugin.plugintype
				print "connectionname: %s" % connectionname
				self.plugin.database = qtsql.QSqlDatabase.addDatabase(drivername,connectionname)
				if not self.plugin.database:
					qt.QMessageBox.critical(self,"Failed to connect","<qt>Failed to create database for driver \"%s\"</qt>" % drivername)
					return False

				hostname = str(self.hostedit.text())
				self.plugin.database.setHostName(hostname)

				portnumber = int(str(self.portedit.text()))
				self.plugin.database.setPort(portnumber)

				username = str(self.useredit.text())
				self.plugin.database.setUserName(username)

				password = str(self.passedit.text())
				self.plugin.database.setPassword(password)

				databasename = str(self.dbedit.text())
				self.plugin.database.setDatabaseName(databasename)

				if not self.plugin.database.open():
					qt.QMessageBox.critical(self,"Failed to connect","<qt>%s<br><br>%s</qt>" % (self.plugin.database.lastError().driverText(),self.plugin.database.lastError().databaseText()))
					return False
				print "database is opened now!"
				self.updateConnectState()
				return True

			def disconnectClicked(self):
				print "trying to disconnect..."
				if self.plugin.database:
					self.plugin.database.close()
					self.plugin.database = None
				print "database is closed now!"
				self.updateConnectState()

		plugin.widget = MainWidget(plugin,self.dialog,parent)
		return plugin.widget
