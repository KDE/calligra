
INCLUDES  = $(MICO_INCLUDES) $(KOFFICE_INCLUDES) $(all_includes) 
LDFLAGS   = $(all_libraries) $(MICO_LDFLAGS) $(KDE_RPATH)
LDADD     = $(LIB_KOFFICEUI) $(LIB_KIMGIO)

#SUBDIRS = formassistent pics toolbar cursors
SUBDIRS = pics toolbar

####### Files

bin_PROGRAMS = kformeditor

kformeditor_SOURCES = \
 kformeditor.idl \
 kformeditor_doc.cc \
 kformeditor_view.cc \
 kformeditor_main.cc \
 kformeditor_shell.cc \
 form.cc \
 formregion.cc \
 formobject.cc \
 formarea.cc \
 areahead.cc \
 widgetwrapper.cc \
 resizewidget.cc \
 backgrounddlg.cc \
 formpropertydlg.cc \
 formsizedlg.cc

noinst_HEADERS = \
 kformeditor.h \
 kformeditor_main.h \
 kformeditor_shell.h \
 kformeditor_doc.h \
 kformeditor_view.h \
 form.h \
 formregion.h \
 formarea.h \
 areahead.h \
 formobject.h \
 widgetwrapper.h \
 resizewidget.h \
 backgrounddlg.h \
 formpropertydlg.h \
 formsizedlg.h

METASOURCES = \
 kformeditor_main.moc \
 kformeditor_shell.moc \
 kformeditor_doc.moc \
 kformeditor_view.moc \
 form.moc \
 formregion.moc \
 formarea.moc \
 areahead.moc \
 backgrounddlg.moc \
 formpropertydlg.moc \
 formsizedlg.moc \
 resizewidget.moc \
 widgetwrapper.moc

idl_DATA = kformeditor.idl

kdelnk_DATA = kformeditor.desktop
kdelnkdir = $(kde_appsdir)/KOffice

####### Implicit rules

####### Build rules

IDL_FLAGS = --query-server-for-narrow --relative-paths -I$(top_srcdir)/lib/kofficecore -I$(top_srcdir)/lib/store -I$(interfacedir) -I$(idldir)

messages:
	(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot *.cc && mv messages.po ../po/kformeditor.pot

