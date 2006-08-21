#!/usr/bin/env python

"""
  Unittest python script to test the Kross KSpread plugin functionality.
"""

import unittest

class TestScripting(unittest.TestCase):

	def setUp(self):
		import KSpreadDocument
		self.KSpreadDocument = KSpreadDocument

		import KSpreadView
		self.KSpreadView = KSpreadView

	def testDocument(self):
		# Display methods the document provides.
		#print "KSpreadDocument: %s" % dir(self.KSpreadDocument)
		pass
	
	def testView(self):
		# Display methods the object provides.
		#print "KSpreadView: %s" % dir(self.KSpreadView)

		# Each object may have a list of child-objects. In fact
		# we are able to access the children either as sequence
		# or as map.
		#for childobject in self.KSpreadView:
		#	print "%s" % childobject
		#self.KSpreadView[idx]

		# Each slot could be called just as method. Arguments
		# will be serialized as well as the returnvalue. Also
		# object-instances are on the fly translated/accessible.
		self.KSpreadView.lastSheet()

	def testViewAdaptor(self):
		# We are able to access the children an object has with mapping
		# using either the objectName or the className as key. So, let's
		# fetch the DBus-ViewAdaptor instance.
		viewadaptor = self.KSpreadView["KSpread::ViewAdaptor"]

		# Call the sheet()-slot to print the name of the sheet.
		print "Sheetname: %s" % viewadaptor.sheet()

#print "__name__ = %s" % __name__
#print "__main__ = %s %s" % (__main__,dir(__main__))
#print "========> %s" % TestObject3.name()

suite = unittest.makeSuite(TestScripting)
unittest.TextTestRunner(verbosity=2).run(suite)
