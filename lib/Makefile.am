
SUBDIRS = koml store kofficecore kofficeui

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot -x $(includedir)/corba.pot  */*.cc && mv messages.po ../po/koffice.pot
