
SUBDIRS = koml store kofficecore kofficeui kscript

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot */*.cc && mv messages.po $(podir)/koffice.pot
