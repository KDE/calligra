#!/usr/bin/env python

"""
  Unittest python script to test the KSpread scripting plugin functionality.
"""

import unittest

class TestScripting(unittest.TestCase):

	def setUp(self):
		# Import the KSpread module to access KSpread.
		import KSpread
		self.KSpread = KSpread

	def testKSpread(self):
		print "===========================> testKSpread"

		# Print methods the KSpread module provides.
		#print "KSpread: %s" % dir(self.KSpread)

		# Print a list of avaible sheets.
		#print "KSpread.sheetNames: " % self.KSpread.sheetNames()

		# Each object may have a list of child-objects. In fact
		# we are able to access the children either as sequence
		# or as map.
		#for idx in range( len(self.KSpread) ):
		#	print "child: %s" % self.KSpread[idx]

	def testApplicationAdaptor(self):
		print "===========================> testApplicationAdaptor"

		# Fetch the ApplicationAdaptor.
		#application = self.KSpread.application()

		# Print name and methods the ApplicationAdaptor provides.
		#print "self.KSpread.application: %s %s" % (application,dir(application))

	def testDocumentAdaptor(self):
		print "===========================> testDocumentAdaptor"

		# Fetch the DocumentAdaptor.
		#document = self.KSpread.document()

		# Print name and methods the DocumentAdaptor provides.
		#print "self.KSpread.document: %s %s" % (document,dir(document))

		# Print some details via some methods from the DocumentAdaptor object.
		#print "self.KSpread.document url=%s isModified=%s actions=%s" % (document.url(),document.isModified(),document.actions())

	def testViewAdaptor(self):
		print "===========================> testViewAdaptor"

		# Fetch the ViewAdaptor.
		#view = self.KSpread.view()

		# Print the methods the ViewAdaptor provides.
		#print "KSpread.view: %s" % dir(view)

		# Activate sheet2.
		#view.showSheet("sheet2")

		# Activate the next sheet.
		#view.nextSheet()

	def testSheetAdaptor(self):
		print "===========================> testSheetAdaptor"

		# Get the current sheet.
		#sheet = self.KSpread.currentSheet()

		# Print the methods the SheetAdaptor provides.
		#print "KSpread.sheet: %s" % dir(sheet)

		# Print some infos about the sheet.
		#name = sheet.name()
		#height = sheet.paperHeight()
		#width = sheet.paperWidth()
		#print "KSpread.sheet name=%s paperHeight=%s paperWidth=%s" % (name,height,width)

		# Change the name to uppercase.
		#sheet.setSheetName( str( sheet.name() ).upper() )

	#def testViewAdaptor(self):
		## We are able to access the children an object has with mapping
		## using either the objectName or the className as key. So, let's
		## fetch the DBus-ViewAdaptor instance.
		#viewadaptor = self.KSpreadView["KSpread::ViewAdaptor"]

		## Call the sheet()-slot to print the name of the sheet.
		#print "Sheetname: %s" % viewadaptor.sheet()

#print "__name__ = %s" % __name__
#print "__main__ = %s %s" % (__main__,dir(__main__))
#print "========> %s" % TestObject3.name()

suite = unittest.makeSuite(TestScripting)
unittest.TextTestRunner(verbosity=2).run(suite)
