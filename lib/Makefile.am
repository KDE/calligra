
SUBDIRS = koml store kofficecore kofficeui kscript

messages:
	$(EXTRACTRC) */*.rc > parts.cc
	$(XGETTEXT) parts.cc */*.cc -o $(podir)/koffice.pot
	rm parts.cc
