
INCLUDES = $(KOFFICE_INCLUDES) $(KOFFICECORE_INCLUDES) \
	$(all_includes) $(MICO_INCLUDES) -I$(idldir)
LDFLAGS	= $(all_libraries) $(PYTHONLIB) $(MICO_LDFLAGS) $(KDE_RPATH)
LDADD = $(LIB_KOFFICECORE)

####### Files

bin_PROGRAMS = kword_winword97.bin

kword_winword97_bin_SOURCES = main.cc

noinst_HEADERS = main.h

METASOURCES = main.moc

DISTCLEANFILES = $(idl_DATA:.idl=.cc) $(idl_HEADERS) $(METASOURCES)

idl_DATA =
idl_HEADERS =

kdelnk_DATA = kword_winword97_import.kdelnk
kdelnkdir = $(kde_datadir)/koffice/filterlnk

datasdir = $(kde_datadir)/koffice/filter

data_DATA = winword97.kdelnk
datadir = $(kde_mimedir)/application


####### Implicit rules

####### Build rules

install-exec-local:
	$(INSTALL_SCRIPT) $(srcdir)/kword_winword97 $(bindir)/kword_winword97

messages:
#	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot  *.cc && mv messages.po ../../kspread_filter.pot

main.cc: main.h
