"""
Initializer for the krosskexiapp-module.

Description:
This module provides the entry-point for python scripts
to work with a running Kexi application instance.

Author:
Sebastian Sauer <mail@dipe.org>

Copyright:
Dual-licensed under LGPL v2+higher and the BSD license.
"""

try:
	import krosskexiapp
except ImportError, e:
	raise "Import of the Kross KexiApp module failed.\n%s" % e

def get(modulename):
	return krosskexiapp.get(modulename)

def currentConnection():
	mainwindow = krosskexiapp.get("KexiAppMainWindow")
	return mainwindow.getConnection()
