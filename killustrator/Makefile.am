# This file you have to edit. Change the name here
SUBDIRS = share filter xmlutils standalone pics koffice

# not a GNU package. You can remove this line, if 
# have all needed files, that a GNU package needs
AUTOMAKE_OPTIONS = foreign

myown_dist = 

# only Tom may know, why make dist forgets configure
# myown_dist lists some more files when a standalone package
EXTRA_DIST = README kil.dtd killustrator.lsm $(myown_dist)

messages:
	$(XGETTEXT) -C -ktranslate -ki18n -kI18N -kSI18N -x$(includedir)/kde.pot */*.cc && mv messages.po ../po/killustrator.pot

