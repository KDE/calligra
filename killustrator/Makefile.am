# This file you have to edit. Change the name here
SUBDIRS = share filter xmlutils standalone koffice pics doc

# not a GNU package. You can remove this line, if 
# have all needed files, that a GNU package needs
AUTOMAKE_OPTIONS = foreign

# only Tom may know, why make dist forgets configure
EXTRA_DIST = README sample.wmf sample2.wmf arrow01.wmf BUGS kiml.dtd killustrator.lsm killustrator.gif configure

messages:
	$(XGETTEXT) -C -ktranslate -ki18n -x$(includedir)/kde.pot */*.cc && mv messages.po ../po/killustrator.pot
