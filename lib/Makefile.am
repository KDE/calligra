
SUBDIRS = koml store kofficecore kofficeui kscript kformula

messages:
	$(EXTRACTRC) */*.rc > parts.cc
	$(XGETTEXT) parts.cc */*.cc -o $(podir)/koffice.pot
	rm parts.cc
