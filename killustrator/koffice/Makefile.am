
INCLUDES  = $(KOFFICE_INCLUDES) -I$(srcdir)/.. -I$(srcdir)/../share -I$(srcdir)/../filter $(MICO_INCLUDES) $(all_includes) -I$(idldir)
LDFLAGS   = $(all_libraries) $(MICO_LDFLAGS) $(KDE_RPATH)
FILTER_LIBS = ./../filter/libfilter.a
XML_LIBS = ./../xmlutils/libxml.a
KSHARE_LIBS = ./../share/libkillu.a
LDADD = $(FILTER_LIBS) $(KSHARE_LIBS) $(XML_LIBS) $(LIB_KOFFICEUI) $(LIB_KIMGIO)

####### Files

bin_PROGRAMS = killustrator

killustrator_SOURCES = KIllustrator_main.cc \
	KIllustrator_doc.cc \
	KIllustrator_shell.cc \
	KIllustrator_view.cc \
	KIllustrator.idl \
	InsertPartTool.cc \
	GPart.cc \
	GfxWrapper.cc

noinst_HEADERS = KIllustrator_main.h \
	KIllustrator_doc.h \
	KIllustrator_shell.h \
	KIllustrator_view.h \
	InsertPartTool.h \
	GPart.h \
	GfxWrapper.h

METASOURCES = KIllustrator_main.moc \
	KIllustrator_shell.moc \
	KIllustrator_doc.moc \
	KIllustrator_view.moc \
	InsertPartTool.moc \
	GPart.moc

idl_DATA = KIllustrator.idl

kdelnk_DATA = killustrator.desktop
kdelnkdir = $(kde_appsdir)/KOffice

IDL_FLAGS = --query-server-for-narrow --relative-paths -I$(top_srcdir)/lib/kofficecore -I$(top_srcdir)/lib/store -I$(idldir)

