
INCLUDES  = $(MICO_INCLUDES) $(KOFFICE_INCLUDES) $(all_includes) -I$(idldir)
LDFLAGS   = $(all_libraries) $(MICO_LDFLAGS) $(KDE_RPATH)
LDADD     = $(LIB_KOFFICEUI) $(LIB_KIMGIO)

####### Files

bin_PROGRAMS = kformviewer

kformviewer_SOURCES = \
 kformviewer.cc \
 kformviewer_main.cc \
 kformviewer_shell.cc \
 kformviewer_doc.cc \
 kformviewer_view.cc

noinst_HEADERS = \
 kformviewer.h \
 kformviewer_main.h \
 kformviewer_shell.h \
 kformviewer_doc.h \
 kformviewer_view.h

METASOURCES = \
 kformviewer_main.moc \
 kformviewer_shell.moc \
 kformviewer_doc.moc \
 kformviewer_view.moc

idl_DATA = kformviewer.idl

DISTCLEANFILES = $(idl_DATA:.idl=.cc) $(idl_HEADERS) $(METASOURCES)

kdelnk_DATA = kformviewer.desktop

kdelnkdir = $(kde_appsdir)/KOffice

####### Implicit rules

####### Build rules

IDL_FLAGS = --query-server-for-narrow --relative-paths -I$(top_srcdir)/lib/kofficecore -I$(top_srcdir)/lib/store -I$(interfacedir) -I$(idldir)

messages:
	(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot *.cc && mv messages.po ../po/kformviewer.pot

kformviewer.h: kformviewer.idl $(micodir)/include/CORBA.h
	$(IDL) $(IDL_FLAGS) $(srcdir)/kformviewer.idl

kformviewer.cc: kformviewer.h
kformviewer_doc.cc: kformviewer.h
kformviewer_main.cc: kformviewer.h
kformviewer_shell.cc: kformviewer.h
kformviewer_view.cc: kformviewer.h
