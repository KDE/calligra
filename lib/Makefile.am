
SUBDIRS = koml store kofficecore kofficeui kscript

messages:
	$(EXTRACTRC) */*.rc > parts.cc
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot parts.cc */*.cc && mv messages.po $(podir)/koffice.pot
	rm parts.cc
