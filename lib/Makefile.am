
SUBDIRS = koml store kofficecore kofficeui koscript kformula

messages:
	$(EXTRACTRC) */*.rc > parts.cc
	$(XGETTEXT) parts.cc */*.cc -o $(podir)/koffice.pot
	rm parts.cc
