
INCLUDES  = $(PYTHONINC) $(MICO_INCLUDES) $(KOFFICE_INCLUDES) -I$(interfacedir) $(all_includes) -I$(idldir)
LDFLAGS   = $(all_libraries) $(MICO_LDFLAGS) $(KDE_RPATH)
LDADD = $(KOFFICE_LIBS) $(LIB_KIMGIO) $(PYTHONLIB) -lkparts -lkpartsui \
	-lkom $(LIB_KIO) $(LIBMICO) $(LIBPYTHON) $(LIB_KFILE)

####### Files

bin_PROGRAMS = koshell.bin

koshell_bin_SOURCES = koshell_main.cc koshell_shell.cc

noinst_HEADERS = koshell_main.h koshell_shell.h

METASOURCES = koshell_main.moc koshell_shell.moc

#idl_DATA = koshell.idl
#idl_HEADERS = koshell.h

DISTCLEANFILES = $(idl_DATA:.idl=.cc) $(idl_HEADERS) $(METASOURCES)

#kdelnk_DATA = koshell_part.kdelnk
#kdelnkdir = $(kde_datadir)/koffice/partlnk

####### Implicit rules

####### Build rules

IDL_FLAGS = --query-server-for-narrow --relative-paths -I$(top_srcdir)/lib/kofficecore -I$(top_srcdir)/lib/store -I$(top_srcdir)/lib/interfaces -I$(idldir)

install-exec-local:
	$(INSTALL_SCRIPT) $(srcdir)/koshell $(bindir)/koshell

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot -x $(includedir)/corba.pot  *.cc && mv messages.po ../po/koshell.pot

# koshell.h: koshell.idl
#	$(IDL) $(IDL_FLAGS) koshell.idl

