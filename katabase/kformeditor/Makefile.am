
INCLUDES  = $(MICO_INCLUDES) $(KOFFICE_INCLUDES) $(all_includes) 
LDFLAGS   = $(all_libraries) $(MICO_LDFLAGS) $(KDE_RPATH)
LDADD     = $(LIB_KOFFICEUI) $(LIB_KIMGIO)

####### Files

bin_PROGRAMS = kformeditor

kformeditor_SOURCES = \
 kformeditor.idl \
 kformeditor_doc.cc \
 kformeditor_view.cc \
 kformeditor_main.cc \
 kformeditor_shell.cc \
 widgetwrapper.cc \
 formobject.cc

noinst_HEADERS = \
 kformeditor.h \
 kformeditor_main.h \
 kformeditor_shell.h \
 kformeditor_doc.h \
 kformeditor_view.h \
 widgetwrapper.h \
 formobject.h

METASOURCES = \
 kformeditor_main.moc \
 kformeditor_shell.moc \
 kformeditor_doc.moc \
 kformeditor_view.moc \
 widgetwrapper.moc

idl_DATA = kformeditor.idl

kdelnk_DATA = kformeditor.desktop
kdelnkdir = $(kde_appsdir)/KOffice

####### Implicit rules

####### Build rules

IDL_FLAGS = --query-server-for-narrow --relative-paths -I$(top_srcdir)/lib/kofficecore -I$(top_srcdir)/lib/store -I$(interfacedir) -I$(idldir)

messages:
	(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot *.cc && mv messages.po ../po/kformeditor.pot

