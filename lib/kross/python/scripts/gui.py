""" 
Python script for a GUI-dialog.

Description:
Python script to provide an abstract GUI for other python scripts. That
way we've all the GUI-related code within one single file and are
able to easily modify GUI-stuff in a central place.

Author:
Sebastian Sauer <mail@dipe.org>

Copyright:
Published as-is without any warranties.
"""

def getHome():
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

class TkDialog:
	""" This class is used to wrap Tkinter into a more abstract interface."""

	def __init__(self, title):
		import Tkinter
		self.root = Tkinter.Tk()
		self.root.title(title)
		self.root.deiconify()

		mainframe = self.Frame(self, self.root)
		self.widget = mainframe.widget

	class Widget:
		def __init__(self, dialog, parent):
			self.dialog = dialog
			self.parent = parent
		#def setVisible(self, visibled): pass
		#def setEnabled(self, enabled): pass

	class Frame(Widget):
		def __init__(self, dialog, parent):
			#TkDialog.Widget.__init__(self, dialog, parent)
			import Tkinter
			self.widget = Tkinter.Frame(parent)
			self.widget.pack()

	class Label(Widget):
		def __init__(self, dialog, parent, caption):
			#TkDialog.Widget.__init__(self, dialog, parent)
			import Tkinter
			self.widget = Tkinter.Label(parent, text=caption)
			self.widget.pack(side=Tkinter.TOP)

	class CheckBox(Widget):
		def __init__(self, dialog, parent, caption, checked = True):
			#TkDialog.Widget.__init__(self, dialog, parent)
			import Tkinter
			self.checkstate = Tkinter.IntVar()
			self.checkstate.set(checked)
			self.widget = Tkinter.Checkbutton(parent, text=caption, variable=self.checkstate)
			self.widget.pack(side=Tkinter.TOP)
		def isChecked(self):
			return self.checkstate.get()

	class List(Widget):
		def __init__(self, dialog, parent, caption, items):
			#TkDialog.Widget.__init__(self, dialog, parent)
			import Tkinter

			listframe = Tkinter.Frame(parent)
			listframe.pack()

			Tkinter.Label(listframe, text=caption).pack(side=Tkinter.LEFT)

			self.items = Tkinter.StringVar()
			itemlist = apply(Tkinter.OptionMenu, (listframe, self.items) + tuple( items ))
			itemlist.pack(side=Tkinter.LEFT)
		def get(self):
			return self.items.get()

	class Button(Widget):
		def __init__(self, dialog, parent, caption, commandmethod):
			#TkDialog.Widget.__init__(self, dialog, parent)
			import Tkinter
			self.widget = Tkinter.Button(parent, text=caption, command=self.doCommand)
			self.commandmethod = commandmethod
			self.widget.pack(side=Tkinter.LEFT)
		def doCommand(self):
			try:
				self.commandmethod()
			except:
				#TODO why the heck we arn't able to redirect exceptions?
				import traceback
				import StringIO
				fp = StringIO.StringIO()
				traceback.print_exc(file=fp)
				import tkMessageBox
				tkMessageBox.showerror("Exception", fp.getvalue())
				#self.dialog.root.destroy()

	class Entry(Widget):
		def __init__(self, dialog, parent, caption):
			#TkDialog.Widget.__init__(self, dialog, parent)
			import Tkinter

			self.widget = Tkinter.Frame(parent)
			self.widget.pack()

			label = Tkinter.Label(self.widget, text=caption)
			label.pack(side=Tkinter.LEFT)

			self.entrytext = Tkinter.StringVar()
			self.entry = Tkinter.Entry(self.widget, width=36, textvariable=self.entrytext)
			self.entry.pack(side=Tkinter.LEFT)
		def get(self):
			return self.entrytext.get()

	class FileChooser(Entry):
		def __init__(self, dialog, parent, caption, initialfile = None, filetypes = None):
			TkDialog.Entry.__init__(self, dialog, parent, caption)
			import Tkinter

			self.initialfile = initialfile
			self.entrytext.set(initialfile)

			btn = Tkinter.Button(self.widget, text="...", command=self.browse)
			btn.pack(side=Tkinter.LEFT)

			if filetypes:
				self.filetypes = filetypes
			else:
				self.filetypes = (('All files', '*'),)

		def browse(self):
			import os
			text = self.entrytext.get()
			d = os.path.dirname(text) or os.path.dirname(self.initialfile)
			f = os.path.basename(text) or os.path.basename(self.initialfile)

			import tkFileDialog
			file = tkFileDialog.asksaveasfilename(
					   initialdir=d,
					   initialfile=f,
					   #defaultextension='.html',
					   filetypes=self.filetypes
			)
			if file:
				self.entrytext.set( file )

	def show(self):
		self.root.mainloop()
		
	def close(self):
		self.root.destroy()

class QtDialog:
	""" This class is used to wrap pyQt/pyKDE into a more abstract interface."""

	def __init__(self, title):
		import qt
		
		class Dialog(qt.QDialog):
			def __init__(self, parent = None, name = None, modal = 0, fl = 0):
				qt.QDialog.__init__(self, parent, name, modal, fl)
				qt.QDialog.accept = self.accept
				self.layout = qt.QVBoxLayout(self)
				self.layout.setSpacing(6)
				self.layout.setMargin(11)
				
		class Label(qt.QLabel):
			def __init__(self, dialog, parent, caption):
				qt.QLabel.__init__(self, caption, parent)
				
		class Frame(qt.QHBox):
			def __init__(self, dialog, parent):
				qt.QHBox.__init__(self, parent)
				self.widget = self
				self.setSpacing(6)

		class Button(qt.QPushButton):
			#def __init__(self, *args):
			def __init__(self, dialog, parent, caption, commandmethod):
				#apply(qt.QPushButton.__init__, (self,) + args)
				qt.QPushButton.__init__(self, parent)
				self.commandmethod = commandmethod
				self.setText(caption)
				qt.QObject.connect(self, qt.SIGNAL("clicked()"), self.commandmethod)


		class CheckBox(qt.QCheckBox):
			def __init__(self, dialog, parent, caption, checked = True):
				#TkDialog.Widget.__init__(self, dialog, parent)
				qt.QCheckBox.__init__(self, parent)
				self.setText(caption)
				self.setChecked(checked)
			#def isChecked(self):
			#	return self.isChecked()

		class List(qt.QHBox):
			def __init__(self, dialog, parent, caption, items):
				qt.QHBox.__init__(self, parent)
				self.setSpacing(6)
				label = qt.QLabel(caption, self)
				self.combo = qt.QComboBox(self)
				self.setStretchFactor(self.combo, 1)
				label.setBuddy(self.combo)
				for item in items:
					self.combo.insertItem( str(item) )
			def get(self):
				return self.combo.currentText()

		class FileChooser(qt.QHBox):
			def __init__(self, dialog, parent, caption, initialfile = None, filetypes = None):
				#apply(qt.QHBox.__init__, (self,) + args)
				qt.QHBox.__init__(self, parent)
				self.setMinimumWidth(400)

				self.initialfile = initialfile
				self.filetypes = filetypes
				
                                self.setSpacing(6)
				label = qt.QLabel(caption, self)
				self.edit = qt.QLineEdit(self)
				self.edit.setText(self.initialfile)
				self.setStretchFactor(self.edit, 1)
				label.setBuddy(self.edit)
				
				browsebutton = Button(dialog, self, "...", self.browseButtonClicked)
				#qt.QObject.connect(browsebutton, qt.SIGNAL("clicked()"), self.browseButtonClicked)

			def get(self):
				return self.edit.text()

			def browseButtonClicked(self):
				filtermask = ""
				import types
				if isinstance(self.filetypes, types.TupleType):
					for ft in self.filetypes:
						if len(ft) == 1:
							filtermask += "%s\n" % (ft[0])
						if len(ft) == 2:
							filtermask += "%s|%s (%s)\n" % (ft[1],ft[0],ft[1])
				if filtermask == "":
					filtermask = "All files (*.*)"
				else:
					filtermask = filtermask[:-1]
					
				filename = None
				try:
					print "QtDialog.FileChooser.browseButtonClicked() kfile.KFileDialog"
					# try to use the kfile module included in pykde
					import kfile
					filename = kfile.KFileDialog.getOpenFileName(self.initialfile, filtermask, self, "Save to file")
				except:
					print "QtDialog.FileChooser.browseButtonClicked() qt.QFileDialog"
					# fallback to Qt filedialog
					filename = qt.QFileDialog.getOpenFileName(self.initialfile, filtermask, self, "Save to file")
				if filename != None and filename != "":
					self.edit.setText(filename)

		app = qt.qApp
		self.dialog = Dialog(app.mainWidget(), "Dialog", 1)
		self.dialog.setCaption(title)

		self.widget = qt.QVBox(self.dialog)
		self.widget.setSpacing(6)
		self.dialog.layout.addWidget(self.widget)

		self.Frame = Frame
		self.Label = Label
		self.Button = Button
 		self.CheckBox = CheckBox
 		self.List = List
		self.FileChooser = FileChooser
		
	def show(self):
		self.dialog.show()

	def close(self):
		print "QtDialog.close()"
		self.dialog.close()

class Dialog:
	""" Central class that provides abstract GUI-access to the outer world. """

	def __init__(self, title):
		self.dialog = None

		try:
			print "Trying to import PyQt..."
			self.dialog = QtDialog222(title)
			print "PyQt is our toolkit!"
		except:
			try:
				print "Failed to import PyQt. Trying to import TkInter..."
				self.dialog = TkDialog(title)
				print "Falling back to TkInter as our toolkit!"
			except:
				raise "Failed to import GUI-toolkit. Please install the PyQt or the Tkinter python module."

                self.widget = self.dialog.widget

        def show(self):
		self.dialog.show()

	def close(self):
		self.dialog.close()

	def addFrame(self, parentwidget):
		return self.dialog.Frame(self.dialog, parentwidget.widget)

	def addLabel(self, parentwidget, caption):
		return self.dialog.Label(self.dialog, parentwidget.widget, caption)

	def addCheckBox(self, parentwidget, caption, checked = True):
		return self.dialog.CheckBox(self.dialog, parentwidget.widget, caption, checked)

	def addButton(self, parentwidget, caption, commandmethod):
		return self.dialog.Button(self.dialog, parentwidget.widget, caption, commandmethod)

	#def addEntry(self, parentwidget, caption):
	#	return self.dialog.Entry(self.dialog, parentwidget.widget, caption)

	def addFileChooser(self, parentwidget, caption, initialfile = None, filetypes = None):
		return self.dialog.FileChooser(self.dialog, parentwidget.widget, caption, initialfile, filetypes)

	def addList(self, parentwidget, caption, items):
		return self.dialog.List(self.dialog, parentwidget.widget, caption, items)
