if include_KARBON
KOPAINTERDIR = kopainter
endif

SUBDIRS = store kwmf kofficecore kofficeui koscript kotext $(KOPAINTERDIR) kformula

messages:
	$(EXTRACTRC) */*.rc > parts.cc
	$(XGETTEXT) parts.cc */*.cc */*.cpp -o $(podir)/koffice.pot
	rm parts.cc
