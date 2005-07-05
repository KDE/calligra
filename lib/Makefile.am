if compile_lib_KOPAINTER
KOPAINTERDIR = kopainter
endif

SUBDIRS = store kwmf kofficecore kofficeui koscript kotext $(KOPAINTERDIR) kformula koproperty 

messages:
#	$(MAKE) -C kformula -f Makefile.am symbolnames.cc
	EXCLUDE="-path ./koproperty"; \
	LIST=`find . \( \( $$EXCLUDE \) -prune -o -name \*.ui -o -name \*.rc \) -type f | grep -v -e '/\.'`; \
	$(EXTRACTRC) $$LIST > rc.cpp; \
	LIST=`find . \( \( $$EXCLUDE \) -prune -o -name \*.cpp -o -name \*.cc \) -type f | grep -v -e '/\.'`; \
	$(XGETTEXT) $$LIST -o $(podir)/koffice.pot
#	$(EXTRACTRC) */*.rc */*.ui > rc.cpp
#	$(XGETTEXT) kotext/kohyphen/*.cpp */*.cc */*.cpp -o $(podir)/koffice.pot
#	rm -f kformula/symbolnames.cc

include ../admin/Doxyfile.am
