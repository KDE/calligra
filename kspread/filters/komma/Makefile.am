
INCLUDES = $(PYTHONINC) $(KOFFICE_INCLUDES) $(KOFFICECORE_INCLUDES) \
	$(all_includes) $(MICO_INCLUDES) -I$(idldir)
LDFLAGS	= $(all_libraries) $(PYTHONLIB) $(MICO_LDFLAGS) $(KDE_RPATH)
LDADD = $(KOFFICE_LIBS) -lkparts -lkpartsui \
	-lkom $(LIB_KIO) $(LIBMICO) $(LIBPYTHON) \
	$(LIB_KFILE)

####### Files

bin_PROGRAMS = kspread_filter.bin

kspread_filter_bin_SOURCES = main.cc

noinst_HEADERS = main.h

METASOURCES = main.moc

DISTCLEANFILES = $(idl_DATA:.idl=.cc) $(idl_HEADERS) $(METASOURCES)

idl_DATA = 
idl_HEADERS =  

kdelnk_DATA = kspread_filter_import.kdelnk
kdelnkdir = $(kde_datadir)/koffice/filterlnk

datasdir = $(kde_datadir)/koffice/filter
datas_DATA = komma.py

####### Implicit rules

####### Build rules

install-exec-local:
	$(INSTALL_SCRIPT) kspread_filter $(bindir)/kspread_filter

messages: 
#	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot  *.cc && mv messages.po ../../kspread_filter.pot

main.cc: main.h

