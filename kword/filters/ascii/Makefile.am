
INCLUDES = $(KOFFICE_INCLUDES) $(KOFFICECORE_INCLUDES) \
	$(all_includes) $(MICO_INCLUDES) -I$(idldir)
LDFLAGS	= $(all_libraries) $(PYTHONLIB) $(MICO_LDFLAGS) $(KDE_RPATH)
LDADD = $(LIB_KOFFICECORE)

####### Files

bin_PROGRAMS = kword_ascii.bin

kword_ascii_bin_SOURCES = main.cc

noinst_HEADERS = main.h

METASOURCES = main.moc

DISTCLEANFILES = $(idl_DATA:.idl=.cc) $(idl_HEADERS) $(METASOURCES)

idl_DATA =
idl_HEADERS =

kdelnk_DATA = kword_ascii_import.kdelnk
kdelnkdir = $(kde_datadir)/koffice/filterlnk

datasdir = $(kde_datadir)/koffice/filter

bin_SCRIPTS = kword_ascii

messages:
#	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot  *.cc && mv messages.po ../../kspread_filter.pot

main.cc: main.h

