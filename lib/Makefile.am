if compile_filter_KARBON
KOPAINTERDIR = kopainter
endif
if include_KOSPELL
KOSPELLDIR = kospell
endif

SUBDIRS = store kwmf kofficecore kofficeui koscript kotext $(KOPAINTERDIR) kformula $(KOSPELLDIR)

messages:
	$(MAKE) -C kformula -f Makefile.am symbolnames.cc
	$(EXTRACTRC) */*.rc */*.ui > parts.cc
	$(XGETTEXT) parts.cc */*.cc */*.cpp -o $(podir)/koffice.pot
	rm parts.cc kformula/symbolnames.cc

include ../admin/Doxyfile.am
