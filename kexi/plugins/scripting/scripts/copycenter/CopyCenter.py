"""
Copy Center

Description:
Python script to copy data between different datastores.

Author:
Sebastian Sauer <mail@dipe.org>

Copyright:
Dual-licensed under LGPL v2+higher and the BSD license.
"""

class CopyCenter:

	class Plugin:
		def __init__(self, plugin):
			self.plugin = plugin
			self.name = plugin.name
			self.source = self.load("Source")
			self.destination = self.load("Destination")

		def load(self, plugintype):
			instance = None
			try:
				if hasattr(self.plugin, plugintype):
					return getattr(self.plugin, plugintype)(self.plugin)
			except:
				import traceback
				print "".join( traceback.format_exception(sys.exc_info()[0],sys.exc_info()[1],sys.exc_info()[2]) )
			return None

	def __init__(self, scriptpath):
		self.scriptpath = scriptpath
		self.homepath = self.getHomePath()
		self.plugins = {}

		import os
		import sys
		if not os.path.exists(scriptpath):
			print "The Path %s does not exist" % scriptpath
		else:
			import re
			regexp = re.compile('^CopyCenterPlugin(.*)\\.py$')
			for f in os.listdir(scriptpath):
				file = os.path.join(scriptpath, f)
				if not os.path.isfile(file): continue
				m = regexp.match(f)
				if not m: continue
				print "Plugin name=%s file=%s" % (m.group(1),file)
				mylocals = {}
				try:
					execfile(file, globals(), mylocals)
					if mylocals.has_key("CopyCenterPlugin"):
						plugin = mylocals.get("CopyCenterPlugin")(self)
						self.plugins[plugin.name] = self.Plugin(plugin)
				except:
					print "Failed to import file=%s" % file
					import traceback
					print "".join( traceback.format_exception(sys.exc_info()[0],sys.exc_info()[1],sys.exc_info()[2]) )

	def getHomePath(self):
		""" Return the homedirectory. """
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

class Copierer:
	def __init__(self): pass
	def appendProgressMessage(self,messagetext): pass
	def writeSuccess(self,record,rowcount): pass
	def writeFailed(self,record): pass

def runGuiApp(copycenter, name):
	import qt
	import sys

	#--------------------------------------------------------------------

	class ListViewDialog(qt.QDialog):
		def __init__(self, parent, caption):
			qt.QDialog.__init__(self, parent, "ProgressDialog", 1)
			self.parent = parent
			self.setCaption(caption)
			layout = qt.QVBoxLayout(self)
			box = qt.QVBox(self)
			box.setMargin(2)
			layout.addWidget(box)
			self.listview = qt.QListView(box)
			self.listview.setAllColumnsShowFocus(True)
			self.listview.header().setStretchEnabled(True,0)
			btnbox = qt.QHBox(box)
			btnbox.setMargin(6)
			btnbox.setSpacing(6)
			self.okbtn = qt.QPushButton(btnbox)
			self.okbtn.setText("Ok")
			#qt.QObject.connect(okbtn, qt.SIGNAL("clicked()"), self.okClicked)
			self.cancelbtn = qt.QPushButton(btnbox)
			self.cancelbtn.setText("Cancel")
			qt.QObject.connect(self.cancelbtn, qt.SIGNAL("clicked()"), self.close)
			box.setMinimumSize(qt.QSize(460,380))
		def addItem(self,valuelist,afteritem = None):
			if afteritem == None:
				item = qt.QListViewItem(self.listview)
			else:
				item = qt.QListViewItem(self.listview,afteritem)
			i = 0
			for value in valuelist:
				item.setText(i,value)
				i += 1
			return item

	#--------------------------------------------------------------------

	class CopyJobWidget(qt.QVBox):
		def __init__(self,dialog,parent):
			self.dialog = dialog
			qt.QVBox.__init__(self,parent)
			self.setSpacing(6)
			typebox = qt.QHBox(self)
			typebox.setSpacing(6)
			label = qt.QLabel("Job File:",typebox)
			self.jobfilecombobox = qt.QComboBox(typebox)
			typebox.setStretchFactor(self.jobfilecombobox,1)
			self.jobfilecombobox.setEditable(True)
			self.jobfilecombobox.insertItem("")
			label.setBuddy(self.jobfilecombobox)
			qt.QObject.connect(self.jobfilecombobox, qt.SIGNAL("textChanged(const QString&)"), self.jobfilecomboboxChanged)

			import os
			import re
			for f in os.listdir(self.dialog.copycenter.homepath):
				file = os.path.join(self.dialog.copycenter.homepath,f)
				if os.path.isfile(file) and re.search(".+\\.copycenterjob.xml$",f):
					self.jobfilecombobox.insertItem(file)

			loadbtn = qt.QPushButton(typebox)
			loadbtn.setText("Open...")
			qt.QObject.connect(loadbtn, qt.SIGNAL("clicked()"), self.openClicked)
			savebtn = qt.QPushButton(typebox)
			savebtn.setText("Save...")
			qt.QObject.connect(savebtn, qt.SIGNAL("clicked()"), self.saveClicked)

			self.listview = qt.QListView(self)
			self.listview.setAllColumnsShowFocus(True)
			self.listview.setSorting(-1)
			self.listview.setDefaultRenameAction(qt.QListView.Reject)
			self.listview.header().setClickEnabled(False)
			self.listview.addColumn("Name")
			self.listview.addColumn("Value")
			qt.QObject.connect(self.listview, qt.SIGNAL("doubleClicked(QListViewItem*, const QPoint&, int)"), self.doubleClicked)
			#qt.QObject.connect(self.listview, qt.SIGNAL("itemRenamed(QListViewItem*, int, const QString&)"), self.itemRenamed)

		def doubleClicked(self, **args):
			print "CopyJobWidget.doubleClicked"
			item = self.listview.selectedItem()
			if item and item.parent(): item.startRename(1)

		def readOptions(self,domnode,plugininst):
			print "CopyJobWidget.readOptions plugintype=\"%s\"" % plugininst.plugintype
			for node in domnode.childNodes:
				if node.nodeType == node.ELEMENT_NODE:
					v = node.getAttribute("value")
					plugininst.options[node.nodeName] = v
					print "Option \"%s\" has value \"%s\" now." % (node.nodeName, v)

		def jobfilecomboboxChanged(self, **args):
			print "CopyJobWidget.jobfilecomboboxChanged"
			import os
			import xml.dom.minidom
			filename = str(self.jobfilecombobox.currentText())
			if not os.path.isfile(filename): return
			domdoc = xml.dom.minidom.parse(filename)
			try:
				elements = domdoc.getElementsByTagName("CopyCenterJob")[0]
				sourcenode = elements.getElementsByTagName("Source")[0]
				destinationnode = elements.getElementsByTagName("Destination")[0]
			except:
				raise "The XML-file \"%s\" does not contain a valid copy-job." % filename
			
			sourcepluginname = str(sourcenode.getAttribute('plugin'))
			if not self.dialog.sourcedata.combobox.listBox().findItem(sourcepluginname,qt.Qt.ExactMatch):
				raise "There exists no plugin with the name \"%s\"." % sourcepluginname
			self.dialog.sourcedata.combobox.setCurrentText(sourcepluginname)

			destinationpluginname = str(destinationnode.getAttribute('plugin'))
			if not self.dialog.destinationdata.combobox.listBox().findItem(destinationpluginname,qt.Qt.ExactMatch):
				raise "There exists no plugin with the name \"%s\"." % destinationpluginname
			self.dialog.destinationdata.combobox.setCurrentText(destinationpluginname)

			self.readOptions(sourcenode,self.dialog.getSourcePluginImpl())
			self.readOptions(destinationnode,self.dialog.getDestinationPluginImpl())
			self.maybeUpdate()

		def openClicked(self):
			text = str(self.jobfilecombobox.currentText())
			if text == "": text = self.dialog.copycenter.homepath
			filename = str(qt.QFileDialog.getOpenFileName(text,"*.copycenterjob.xml;;*",self.dialog))
			if filename != "": self.jobfilecombobox.setCurrentText(filename)

		def escape(self,s):
			return s.replace("&", "&amp;").replace("'", "&apos;").replace("<", "&lt;").replace(">", "&gt;").replace('"', "&quot;")

		def writeOptions(self,writer,pluginname,plugininst):
			print "CopyJobWidget.writeOptions"
			writer.write("<%s plugin=\"%s\">\n" % (plugininst.plugintype, pluginname))
			for optionname in plugininst.options:
				value = self.escape( unicode(plugininst.options[optionname]).encode("utf-8") )
				writer.write("\t<%s value=\"%s\" />\n" % (optionname,value))
			writer.write("</%s>\n" % plugininst.plugintype)

		def saveClicked(self):
			text = str(self.jobfilecombobox.currentText())
			if text == "":
				import os
				text = os.path.join(self.dialog.copycenter.homepath,"default.copycenterjob.xml")
			filename = str(qt.QFileDialog.getSaveFileName(text,"*.copycenterjob.xml;;*",self.dialog))
			if str(filename) == "": return
			f = open(filename, "w")
			f.write("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n")
			f.write("<CopyCenterJob>\n")
			sourcepluginname = self.dialog.sourcedata.combobox.currentText()
			self.writeOptions(f, sourcepluginname, self.dialog.getSourcePluginImpl())
			destinationpluginname = self.dialog.destinationdata.combobox.currentText()
			self.writeOptions(f, destinationpluginname, self.dialog.getDestinationPluginImpl())
			f.write("</CopyCenterJob>\n")
			f.close()
			print "File \%s\" successfully written." % filename

		def addItem(self, pluginimpl, afteritem = None, parentitem = None):
			#print "CopyJobWidget.addItem"
			class ListViewItem(qt.QListViewItem):
				def __init__(self, pluginimpl, listview, parentitem = None, afteritem = None):
					self.pluginimpl = pluginimpl
					if parentitem == None:
						qt.QListViewItem.__init__(self,listview)
						self.setOpen(True)
					else:
						if afteritem == None:
							qt.QListViewItem.__init__(self,parentitem)
						else:
							qt.QListViewItem.__init__(self,parentitem,afteritem)
						self.setRenameEnabled(1,True)
				def startRename(self, columnindex):
					qt.QListViewItem.startRename(self,columnindex)
					#lineedit = self.listView().viewport().child("qt_renamebox")
					#if lineedit:
					#	regexp = qt.QRegExp("^[_A-Z]+[_A-Z0-9]*$", False)
					#	v = qt.QRegExpValidator(regexp, self.listView());
					#	lineedit.setValidator(v)
				def okRename(self, columnindex):
					if columnindex == 1:
						n = str(self.text(0))
						if not self.pluginimpl.options.has_key(n):
							raise "No such option \"%s\"" % n
						qt.QListViewItem.okRename(self,columnindex)
						v = str(qt.QListViewItem.text(self,1))
						print "Option \"%s\" has value \"%s\" now." % (n,v)
						self.pluginimpl.options[n] = v

				def text(self, columnindex):
					if columnindex == 1:
						if qt.QListViewItem.text(self,0).contains("password"):
							return "*" * len(str(qt.QListViewItem.text(self,1)))
					return qt.QListViewItem.text(self,columnindex)
			return ListViewItem(pluginimpl, self.listview, parentitem, afteritem)

		def updateItem(self,pluginname,pluginimpl):
			#print "CopyJobWidget.updateItem"
			if pluginimpl == None: return
			#plugin = self.dialog.plugins[pluginname]
			item = self.addItem(pluginimpl)
			item.setText(0,"%s: %s" % (pluginimpl.plugintype, pluginname))
			afteritem = None
			for i in pluginimpl.options:
				afteritem = self.addItem(pluginimpl, afteritem, item)
				afteritem.setText(0,str(i))
				afteritem.setText(1,str(pluginimpl.options[i]))
				print "CopyJobWidget.updateItem Added item with name \"%s\" and value \"%s\"" % (str(i),str(pluginimpl.options[i]))
			pass
		
		def maybeUpdate(self):
			print "CopyJobWidget.maybeUpdate"
			self.listview.clear()
			try:
				self.updateItem(self.dialog.getDestinationPluginName(), self.dialog.getDestinationPluginImpl())
				self.updateItem(self.dialog.getSourcePluginName(), self.dialog.getSourcePluginImpl())
			except:
				import traceback
				print "".join( traceback.format_exception(sys.exc_info()[0],sys.exc_info()[1],sys.exc_info()[2]) )
				self.listview.clear()

	#--------------------------------------------------------------------

	class ProgressDialog(qt.QDialog):
		def __init__(self, dialog):
			self.dialog = dialog
			self.starttime = None
			qt.QDialog.__init__(self, dialog, "ProgressDialog", 1)
			self.setCaption("Copying...")
			layout = qt.QVBoxLayout(self)
			box = qt.QVBox(self)
			box.setSpacing(6)
			box.setMargin(6)
			layout.addWidget(box)
			self.textbrowser = qt.QTextBrowser(box)
			self.textbrowser.setWordWrap(qt.QTextEdit.WidgetWidth)
			self.textbrowser.setTextFormat(qt.Qt.RichText)
			statusbox = qt.QFrame(box)
			layout = qt.QGridLayout(statusbox,4,2,0,2)
			layout.addWidget(qt.QLabel("Number of records done:",statusbox),0,0)
			self.donecounter = 0
			self.donelabel = qt.QLabel("-",statusbox)
			layout.addWidget(self.donelabel,0,1)
			layout.addWidget(qt.QLabel("Successfully copied records:",statusbox),1,0)
			self.successcounter = 0
			self.successlabel = qt.QLabel("-",statusbox)
			layout.addWidget(self.successlabel,1,1)
			layout.addWidget(qt.QLabel("Failed to copy records:",statusbox),2,0)
			self.failedcounter = 0
			self.failedlabel = qt.QLabel("-",statusbox)
			layout.addWidget(self.failedlabel,2,1)
			layout.addWidget(qt.QLabel("Elapsed time in seconds:",statusbox),3,0)
			self.elapsedlabel = qt.QLabel("-",statusbox)
			layout.addWidget(self.elapsedlabel,3,1)
			btnbox = qt.QHBox(box)
			btnbox.setSpacing(6)
			self.donebtn = qt.QPushButton(btnbox)
			self.donebtn.setText("Done")
			self.donebtn.setEnabled(False)
			qt.QObject.connect(self.donebtn,qt.SIGNAL("clicked()"),self.close)
			self.cancelbtn = qt.QPushButton(btnbox)
			self.cancelbtn.setText("Cancel")
			qt.QObject.connect(self.cancelbtn,qt.SIGNAL("clicked()"),self.close)
			box.setMinimumSize( qt.QSize(500,380) )

		def updateStates(self):
			if self.starttime != None:
				self.donelabel.setText(str(self.donecounter))
				self.failedlabel.setText(str(self.failedcounter))
				self.successlabel.setText(str(self.successcounter))
				self.elapsedlabel.setText( str(self.starttime.elapsed() / 1000) )
				self.donelabel.update()
				self.failedlabel.update()
				self.successlabel.update()
				self.elapsedlabel.update()

		def writeSuccess(self, record, rowcount):
			self.donecounter += rowcount
			self.successcounter += rowcount
			qt.qApp.processEvents()
		def writeFailed(self, record):
			self.donecounter += 1
			self.failedcounter += 1
			qt.qApp.processEvents()

		def startCopy(self):
			try:
				global Copierer
				copierer = Copierer()
				copierer.appendProgressMessage = self.textbrowser.append
				copierer.writeSuccess = self.writeSuccess
				copierer.writeFailed = self.writeFailed

				self.starttime = qt.QTime()
				self.updatetimer = qt.QTimer(self)
				qt.QObject.connect(self.updatetimer,qt.SIGNAL("timeout()"),self.updateStates)

				# Initialize the source
				sourcename = self.dialog.getSourcePluginName()
				sourceimpl = self.dialog.getSourcePluginImpl()
				self.textbrowser.append("Source: %s" % sourcename)
				if sourceimpl == None:
					raise "No such source."
				try:
					sourceimpl.init(copierer)

					# Initialize the destination
					destinationname = self.dialog.getDestinationPluginName()
					destinationimpl = self.dialog.getDestinationPluginImpl()
					self.textbrowser.append("<hr>Destination: %s" % destinationname)
					if destinationimpl == None:
						raise "No such destination."
					try:
						destinationimpl.init(copierer)

						self.starttime.start()
						self.updatetimer.start(500)
						qt.qApp.processEvents()

						# Copy the records
						self.textbrowser.append("<hr><i>Copy the records...</i>")
						while True:
							record = sourceimpl.read()
							if record == None: break
							destinationimpl.write(record)

						self.updateStates()
					finally:
						destinationimpl.finish()
				finally:
					sourceimpl.finish()

				self.setCaption("Copy done")
				self.textbrowser.append("<hr><b>Copy done.</b>")
			except:
				self.setCaption("Copy failed")
				self.textbrowser.append("<b>Error: %s</b>" % sys.exc_info()[0])
				import traceback
				print "".join( traceback.format_exception(sys.exc_info()[0],sys.exc_info()[1],sys.exc_info()[2]) )
			#self.progressbar.setEnabled(False)
			self.donebtn.setEnabled(True)
			self.cancelbtn.setEnabled(False)
			self.updatetimer.stop()
			self.starttime = None

		def show(self):
			qt.QDialog.show(self)
			qt.QTimer.singleShot(10,self.startCopy)
			qt.qApp.processEvents()

		def closeEvent(self, closeevent):
			if not self.dialog.getSourcePluginImpl().isFinished():
				if qt.QMessageBox.warning(self,"Abort?","Abort the copy?",qt.QMessageBox.Yes,qt.QMessageBox.No) != qt.QMessageBox.Yes:
					closeevent.ignore()
					return
				self.dialog.getSourcePluginImpl().finish()
				self.dialog.getDestinationPluginImpl().finish()
			closeevent.accept()

	#--------------------------------------------------------------------

	class DataSelector(qt.QVGroupBox):
		def __init__(self, plugintype, title, caption, parent, dialog, items):
			self.plugintype = plugintype
			self.pluginimpl = None
			self.dialog = dialog
			self.mainbox = None

			qt.QVGroupBox.__init__(self,title,parent)
			self.setInsideMargin(6)
			self.setInsideSpacing(0)

			typebox = qt.QHBox(self)
			label = qt.QLabel(caption,typebox)
			self.combobox = qt.QComboBox(typebox)
			for item in items:
				self.combobox.insertItem(str(item))
			label.setBuddy(self.combobox)
			typebox.setStretchFactor(self.combobox,1)

			self.scrollview = qt.QScrollView(self)
			try:
				self.scrollview.setResizePolicy(qt.QScrollView.AutoOne)
				self.scrollview.setFrameStyle(qt.QFrame.NoFrame);
				self.scrollview.setResizePolicy(qt.QScrollView.AutoOneFit);
				self.scrollview.viewport().setPaletteBackgroundColor(self.paletteBackgroundColor())
			except:
				import traceback
				print "".join( traceback.format_exception(sys.exc_info()[0],sys.exc_info()[1],sys.exc_info()[2]) )
			qt.QObject.connect(self.combobox, qt.SIGNAL("activated(int)"), self.activated)

		def updatePlugin(self):
			print "DataSelector.updatePlugin"
			self.pluginimpl = None
			text = str(self.combobox.currentText())
			plugin = self.dialog.copycenter.plugins[text]
			self.pluginimpl = getattr(plugin, self.plugintype)

		def removeMainBox(self):
			if self.mainbox == None: return
			try:
				self.scrollview.removeChild(self.mainbox)
				self.mainbox.destroy()
			except:
				pass
			self.mainbox = None

		def updateMainBox(self):
			print "DataSelector.updateMainBox"
			self.removeMainBox()
			self.mainbox = qt.QVBox( self.scrollview.viewport() )
			self.mainbox.setSpacing(2)
			if self.pluginimpl != None:
				try:
					self.pluginimpl.createWidget(self.dialog, self.mainbox)
				except:
					import traceback
					print "".join( traceback.format_exception(sys.exc_info()[0],sys.exc_info()[1],sys.exc_info()[2]) )
			self.mainbox.setStretchFactor(qt.QWidget(self.mainbox), 1)
			self.mainbox.show()
			self.scrollview.addChild(self.mainbox)

		def activated(self, **args):
			self.updatePlugin()
			self.updateMainBox()

		def maybeUpdate(self):
			print "DataSelector.maybeUpdate"
			self.removeMainBox()
			qt.QTimer.singleShot(50, self.activated)

		def maybeDone(self):
			print "DataSelector.maybeDone"
			if self.pluginimpl.widget == None: return
			for optionname in self.pluginimpl.options:
				self.pluginimpl.options[optionname] = self.pluginimpl.widget.getOptionValue(optionname)

	#--------------------------------------------------------------------

	class Dialog(qt.QDialog):
		def __init__(self, copycenter, parent):
			self.copycenter = copycenter

			import qt
			import os
			import sys

			self.ListViewDialog = ListViewDialog
			qt.QDialog.__init__(self, parent, "Dialog", 1, qt.Qt.WDestructiveClose)
			self.setCaption("Copy Center")
			layout = qt.QVBoxLayout(self)
			box = qt.QVBox(self)
			box.setMargin(6)
			box.setSpacing(6)
			layout.addWidget(box)
			self.tab = qt.QTabWidget(box)
			self.tab.setMargin(6)
			box.setStretchFactor(self.tab,1)

			self.jobsbox = CopyJobWidget(self,self.tab)
			self.tab.addTab(self.jobsbox,"Jobs")

			self.splitter = qt.QSplitter(self.tab)

			sourceplugins = []
			destinationplugins = []
			for pluginname in self.copycenter.plugins:
				if self.copycenter.plugins[pluginname].source != None:
					sourceplugins.append(pluginname)
				if self.copycenter.plugins[pluginname].destination != None:
					destinationplugins.append(pluginname)
			sourceplugins.sort()
			destinationplugins.sort()

			self.sourcedata = DataSelector(
				"source", # id
				"Read Data From", # title
				"Source:", # caption
				self.splitter, self, sourceplugins)
			self.destinationdata = DataSelector(
				"destination", # id
				"Write Data to", # title
				"Destination:", # caption
				self.splitter, self, destinationplugins)

			btnbox = qt.QHBox(box)
			btnbox.setSpacing(6)
			okbtn = qt.QPushButton(btnbox)
			okbtn.setText("Start Copy")
			okbtn.setDefault(True)
			qt.QObject.connect(okbtn,qt.SIGNAL("clicked()"),self.startCopy)
			cancelbtn = qt.QPushButton(btnbox)
			cancelbtn.setText("Cancel")
			qt.QObject.connect(cancelbtn,qt.SIGNAL("clicked()"),self.close)

			self.tab.addTab(self.splitter,"Copy")
			self.tab.setCurrentPage(1)

			self.helpbrowser = qt.QTextBrowser(self.tab)
			self.helpbrowser.setLinkUnderline(False)
			self.helpbrowser.setUndoRedoEnabled(False)
			self.tab.addTab(self.helpbrowser,"Help")
			qt.QObject.connect(self.tab,qt.SIGNAL("currentChanged(QWidget*)"),self.currentTabChanged)

			box.setMinimumSize( qt.QSize(760,500) )
			
			defaultfile = os.path.join(self.copycenter.homepath,"default.copycenterjob.xml")
			if os.path.isfile(defaultfile):
				print "Reading default copy job file: %s" % defaultfile
				self.jobsbox.jobfilecombobox.setCurrentText(defaultfile)

		def getSourcePluginName(self):
			return str(self.sourcedata.combobox.currentText())
		def getSourcePluginImpl(self):
			return self.copycenter.plugins[self.getSourcePluginName()].source
		def getDestinationPluginName(self):
			return str(self.destinationdata.combobox.currentText())
		def getDestinationPluginImpl(self):
			return self.copycenter.plugins[self.getDestinationPluginName()].destination

		def currentTabChanged(self,widget):
			if self.tab.currentPage() == self.jobsbox:
				# The "Copy" page is done
				self.sourcedata.maybeDone()
				self.destinationdata.maybeDone()
				# Update the "Jobs" page
				self.jobsbox.maybeUpdate()
			elif self.tab.currentPage() == self.splitter:
				# Update the "Copy" page
				self.sourcedata.maybeUpdate()
				self.destinationdata.maybeUpdate()
			elif self.tab.currentPage() == self.helpbrowser and self.helpbrowser.lines() <= 1:
				# Update the "Help" page
				import os
				file = os.path.join(self.copycenter.scriptpath, "readme.html")
				if not os.path.isfile(file): return
				fh = open(file,'r')
				self.helpbrowser.setText( fh.read() )
				fh.close()
				
		def startCopy(self):
			dlg = ProgressDialog(self)
			dlg.show()

	#--------------------------------------------------------------------

	if name == "__main__":
		qtapp = qt.QApplication(sys.argv)
	else:
		qtapp = qt.qApp
	dialog = Dialog(copycenter, qtapp.mainWidget())
	dialog.exec_loop()

import os

if __name__ == "__main__":
	scriptpath = os.getcwd()
else:
	scriptpath = os.path.dirname(__name__)

copycenter = CopyCenter(scriptpath)
runGuiApp(copycenter, __name__)
