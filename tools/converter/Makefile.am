INCLUDES = $(KOFFICE_INCLUDES) $(all_includes)

bin_PROGRAMS = koconverter
koconverter_SOURCES = koconverter.cpp
koconverter_LDFLAGS = $(all_libraries) $(KDE_RPATH)
koconverter_LDADD = $(top_builddir)/lib/kofficecore/libkofficecore.la

METASOURCES = AUTO

messages:
	$(XGETTEXT) *.cpp -o $(podir)/koconverter.pot
