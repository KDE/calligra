# This file you have to edit. Change the name here
SUBDIRS = src 

# not a GNU package. You can remove this line, if 
# have all needed files, that a GNU package needs
AUTOMAKE_OPTIONS = foreign

EXTRA_DIST = README sample.wmf sample2.wmf arrow01.wmf

messages:
	$(XGETTEXT) -C -ktranslate -ki18n -x$(includedir)/kde.pot src/*/*.cc && mv messages.po ../po/killustrator.pot
