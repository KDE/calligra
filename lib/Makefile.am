
SUBDIRS = store kofficecore kofficeui koscript kwmf kformula2

messages:
	$(EXTRACTRC) */*.rc > parts.cc
	$(XGETTEXT) parts.cc */*.cc */*.cpp -o $(podir)/koffice.pot
	rm parts.cc
