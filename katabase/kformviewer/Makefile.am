
INCLUDES  = $(MICO_INCLUDES) $(KOFFICE_INCLUDES) $(all_includes) 
LDFLAGS   = $(all_libraries) $(MICO_LDFLAGS) $(KDE_RPATH)
LDADD     = $(LIB_KOFFICEUI) $(LIB_KIMGIO) $(LIB_KOFFICECORE) $(LIB_KOML) \
		$(LIB_KSTORE)
####### Files

bin_PROGRAMS = kformviewer

kformviewer_SOURCES = \
 kformviewer.idl \
 kformviewer_doc.cc \
 kformviewer_view.cc \
 kformviewer_main.cc \
 kformviewer_shell.cc \
 formobject.cc

noinst_HEADERS = \
 kformviewer.h \
 kformviewer_main.h \
 kformviewer_shell.h \
 kformviewer_doc.h \
 kformviewer_view.h \
 formobject.h

METASOURCES = \
 kformviewer_main.moc \
 kformviewer_shell.moc \
 kformviewer_doc.moc \
 kformviewer_view.moc

idl_DATA = kformviewer.idl
kformviewer.h: $(top_builddir)/lib/kofficecore/koffice.idl

kdelnk_DATA = kformviewer.desktop
kdelnkdir = $(kde_appsdir)/KOffice

####### Implicit rules

####### Build rules

IDL_FLAGS = --relative-paths -I$(top_srcdir)/lib/kofficecore -I$(top_srcdir)/lib/store -I$(interfacedir) -I$(idldir)

messages:
	(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot *.cc && mv messages.po ../po/kformviewer.pot

