# This file you have to edit. Change the name here
SUBDIRS = ui share filter pics koffice

# not a GNU package. You can remove this line, if
# have all needed files, that a GNU package needs
AUTOMAKE_OPTIONS = foreign

myown_dist =

# only Tom may know, why make dist forgets configure
# myown_dist lists some more files when a standalone package
EXTRA_DIST = README kil.dtd killustrator.lsm $(myown_dist)

messages:
	$(EXTRACTRC) */*.rc >> rc.cpp
	$(XGETTEXT) -kI18N -kSI18N rc.cpp */*.cc -o $(podir)/killustrator.pot
