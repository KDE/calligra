SUBDIRS = lib part shell samples docs


# The following hack means that 'make dist' works as it should.

dist: am_edit-hack

am_edit-hack:
	cd $(distdir) && perl ../am_edit
