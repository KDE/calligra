
SUBDIRS = koml store kofficecore kofficeui koscript kformula kwmf kformula2

messages:
	$(EXTRACTRC) */*.rc > parts.cc
	$(XGETTEXT) parts.cc */*.cc */*.cpp -o $(podir)/koffice.pot
	rm parts.cc
