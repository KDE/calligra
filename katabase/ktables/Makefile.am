
INCLUDES = -I../kdb $(KOFFICE_INCLUDES) $(all_includes) $(MICO_INCLUDES)
LDFLAGS = -g $(all_libraries) $(MICO_LDFLAGS) $(KDE_RPATH)
LDADD = ../../lib/kofficeui/libkofficeui.la ../kdb/libkdb.la

# ../lib/katabasecore/libkatabasecore.la

SUBDIRS = pics

####### Files
 
bin_PROGRAMS = ktables

ktables_SOURCES = ktables.idl tableselectdata.cpp tableselect.cpp \
	ktables_server.cpp ktables_shell.cpp ktables_main.cpp \
	ktables_view.cpp ktables_doc.cpp main.cpp \
	propertyeditor.cpp propertyitem.cpp \
	querydialog.cpp queryselect.cpp

noinst_HEADERS = tableselect.h ktables_server.h ktables_view.h \
	ktables_doc.h ktables_shell.h ktables_main.h \
	propertyeditor.h propertyitem.h resource.h \
	querydialog.h queryselect.h

METASOURCES = tableselect.moc ktables_server.moc ktables_doc.moc \
	ktables_view.moc ktables_shell.moc ktables_main.moc \
	propertyeditor.moc querydialog.moc queryselect.moc 

messages:
	$(XGETTEXT) $(ktables_SOURCES) -o $(podir)/ktables.pot

ktables.h: $(top_builddir)/lib/kofficecore/koffice.idl

IDL_FLAGS = --relative-paths -I$(top_srcdir)/lib/kofficecore -I$(top_srcdir)/lib/store -I$(idldir)

