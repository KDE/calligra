
INCLUDES = $(all_includes) $(MICO_INCLUDES) 
LDFLAGS	= $(all_libraries) $(MICO_LDFLAGS) $(KDE_RPATH)
LDADD = -lkom $(LIB_KIMGIO) -lkspell $(LIBMICO) $(LIB_KFILE)

####### Files

bin_PROGRAMS = spell_tool

spell_tool_SOURCES = main.cc datatools.idl

noinst_HEADERS = main.h datatools.h

METASOURCES = main.moc

service_DATA = spell_tool.desktop
servicedir = $(kde_servicesdir)

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot  *.cc && mv messages.po ../../po/spell_tool.pot

IDL_FLAGS = --query-server-for-narrow --relative-paths --any -I$(idldir)

datatools_IDLDIR = $(idldir)

