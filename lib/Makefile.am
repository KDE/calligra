
SUBDIRS = store kwmf kofficecore kofficeui koscript kformula

messages:
	$(EXTRACTRC) */*.rc > parts.cc
	$(XGETTEXT) parts.cc */*.cc */*.cpp -o $(podir)/koffice.pot
	rm parts.cc
