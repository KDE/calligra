
INCLUDES = $(all_includes) $(MICO_INCLUDES) -I$(idldir)
LDFLAGS	= $(all_libraries) $(MICO_LDFLAGS) $(KDE_RPATH)
LDADD = -lkom $(LIB_KIMGIO) -lkspell $(LIBMICO) $(LIB_KFILE)

####### Files

bin_PROGRAMS = spell_tool

spell_tool_SOURCES = main.cc datatools.cc

noinst_HEADERS = main.h datatools.h

METASOURCES =	main.moc

DISTCLEANFILES = $(idl_DATA:.idl=.cc) $(idl_HEADERS) datatools.h datatools.cc

kdelnk_DATA = spell_tool.kdelnk
kdelnkdir = $(kde_datadir)/koffice/toollnk

####### Implicit rules

####### Build rules

install-exec-local:
	$(INSTALL_SCRIPT) spell_tool $(bindir)/spell_tool

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot  *.cc && mv messages.po ../../po/spell_tool.pot

datatools.h: $(idldir)/datatools.idl
	rm -f datatools.h
	rm -f datatools.cc
	$(IDL) --query-server-for-narrow --relative-paths --any -I$(idldir) $(idldir)/datatools.idl

main.cc: main.h
datatools.cc: datatools.h
