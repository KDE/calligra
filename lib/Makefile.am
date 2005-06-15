if compile_lib_KOPAINTER
KOPAINTERDIR = kopainter
endif

SUBDIRS = store kwmf kofficecore kofficeui koscript kotext $(KOPAINTERDIR) kformula koproperty 

messages:
#	$(MAKE) -C kformula -f Makefile.am symbolnames.cc
	$(EXTRACTRC) */*.rc */*.ui > rc.cpp
	$(XGETTEXT) kotext/kohyphen/*.cpp */*.cc */*.cpp -o $(podir)/koffice.pot
#	rm -f kformula/symbolnames.cc

include ../admin/Doxyfile.am
