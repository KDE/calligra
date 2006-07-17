"""
KSpread Kross Python Script.

Export to HTML File.

Description:
This script exports data from KSpread to a HTML file.

Author:
Sebastian Sauer <mail@dipe.org>

Copyright:
Dual-licensed under LGPL v2+higher and the BSD license.
"""

import os, sys

try:
	import qt
except (ImportError):
	raise "Failed to import the required PyQt python module."

class Dialog(qt.QDialog):
	def __init__(self, scriptpath, parent):
		self.scriptpath = scriptpath

		import krosskspreadcore
		self.doc = krosskspreadcore.get("KSpreadDocument")

		import qt
		qt.QDialog.__init__(self, parent, "Dialog", 1, qt.Qt.WDestructiveClose)
		self.setCaption("Export to HTML File")
		layout = qt.QVBoxLayout(self)
		box = qt.QVBox(self)
		box.setMargin(10)
		box.setSpacing(10)
		layout.addWidget(box)

		sheetbox = qt.QHBox(box)
		sheetbox.setSpacing(6)
		sheetlabel = qt.QLabel("Sheet:",sheetbox)
		self.sheetcombo = qt.QComboBox(sheetbox)
		currentsheetname = self.doc.currentSheet().name()
		for sheetname in self.doc.sheetNames():
			self.sheetcombo.insertItem(sheetname)
			if sheetname == currentsheetname:
				self.sheetcombo.setCurrentItem(self.sheetcombo.count() - 1)
		sheetlabel.setBuddy(self.sheetcombo)
		sheetbox.setStretchFactor(self.sheetcombo,1)

		self.styles = {
			"Paper" :
				"html { background-color:#efefef; }"
				"body { background-color:#fafafa; color:#303030; margin:1em; padding:1em; border:#606060 1px solid; }"
			,
			"Plain" :
				"html { background-color:#ffffff; color:#000; }"
				"body { margin:1em; }"
			,
			"Seawater" :
				"html { background-color:#0000aa; }"
				"body { background-color:#000066; color:#efefff; margin:1em; padding:1em; border:#00f 1px solid; }"
				"h1 { color:#0000ff; }"
				"th { color:#6666ff; }"
			,
		}

		stylebox = qt.QHBox(box)
		stylebox.setSpacing(6)
		stylelabel = qt.QLabel("Style:",stylebox)
		self.stylecombo = qt.QComboBox(stylebox)
		stylenames = self.styles.keys()
		stylenames.sort()
		for stylename in stylenames:
			self.stylecombo.insertItem(stylename)
		stylelabel.setBuddy(self.stylecombo)
		stylebox.setStretchFactor(self.stylecombo,1)

		filebox = qt.QHBox(box)
		filebox.setSpacing(6)
		filelabel = qt.QLabel("File:",filebox)
		self.fileedit = qt.QLineEdit(self.getDefaultFile(),filebox)
		btn = qt.QPushButton("...",filebox)
		qt.QObject.connect(btn, qt.SIGNAL("clicked()"),self.browseClicked)
		filelabel.setBuddy(self.fileedit)
		filebox.setStretchFactor(self.fileedit,1)

		btnbox = qt.QHBox(box)
		btnbox.setSpacing(6)
		okbtn = qt.QPushButton(btnbox)
		okbtn.setText("Export")
		okbtn.setDefault(True)
		qt.QObject.connect(okbtn,qt.SIGNAL("clicked()"),self.startExport)
		cancelbtn = qt.QPushButton(btnbox)
		cancelbtn.setText("Cancel")
		qt.QObject.connect(cancelbtn,qt.SIGNAL("clicked()"),self.close)

		box.setMinimumWidth(480)

	def browseClicked(self):
		import qt
		filename = str( qt.QFileDialog.getSaveFileName(str(self.fileedit.text()),"*.htm *.html *.xhtml;;*", self) )
		if filename != "": self.fileedit.setText(filename)
	
	def getDefaultFile(self):
		import os
		try:
			homepath = os.getenv("HOME")
			if not homepath:
				import pwd
				user = os.getenv("USER") or os.getenv("LOGNAME")
				if not user:
					pwent = pwd.getpwuid(os.getuid())
				else:
					pwent = pwd.getpwnam(user)
				homepath = pwent[6]
		except (KeyError, ImportError):
			homepath = os.curdir
		return os.path.join(homepath, "kspreadexport.html")

	def startExport(self):
		import qt

		sheetname = str( self.sheetcombo.currentText() )
		sheet = self.doc.sheetByName( sheetname )
		print "sheetname=%s sheet=%s" % (sheetname,sheet)

		filename = str( self.fileedit.text() )
		try:
			file = open(filename, "w")
		except IOError, (errno, strerror):
			qt.QMessageBox.critical(self,"Error","<qt>Failed to create HTML file \"%s\"<br><br>%s</qt>" % (filename,strerror))
			return

		file.write("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n")
		file.write("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\" \"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">\n")
		file.write("<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"en\">\n")
		file.write("<head><title>%s</title>\n" % sheetname)

		file.write("<style type=\"text/css\">\n<!--\n")
		file.write( self.styles[ str(self.stylecombo.currentText()) ] )
		file.write("\n//-->\n</style>\n")

		file.write("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />\n")
		file.write("</head><body><h1>%s</h1>\n" % sheetname)

		file.write("<table border=\"1\">\n")
		cell = sheet.firstCell()
		prevrow = -1
		while cell:
			#print "Cell col=%s row=%s value=%s" % (cell.column(),cell.row(),cell.value())

			row = cell.row()
			if row != prevrow:
				prevrow = row
				file.write("<tr>")
				file.write("<th>%s</th>" % row)

			file.write("<td>%s</td>" % cell.value())

			cell = cell.nextCell()
			if cell == None or cell.row() != prevrow:
				file.write("</tr>\n")

		file.write("</table>\n")
		file.write("</body></html>\n")

		file.close()
		self.close()

if __name__ == "__main__":
	scriptpath = os.getcwd()
	qtapp = qt.QApplication(sys.argv)
else:
	scriptpath = os.path.dirname(__name__)
	qtapp = qt.qApp

dialog = Dialog(scriptpath, qtapp.mainWidget())
dialog.exec_loop()
