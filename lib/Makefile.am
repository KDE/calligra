if include_KARBON
KOPAINTERDIR = kopainter
endif

SUBDIRS = store kwmf kofficecore kofficeui koscript kotext $(KOPAINTERDIR) kformula

messages:
	$(MAKE) -C kformula -f Makefile.am symbolnames.cc
	$(EXTRACTRC) */*.rc */*.ui > parts.cc
	$(XGETTEXT) parts.cc */*.cc */*.cpp -o $(podir)/koffice.pot
	rm parts.cc kformula/symbolnames.cc

include ../admin/Doxyfile.am
