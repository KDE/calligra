####### General stuff

INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) -I$(srcdir)/../libexport $(all_includes)

####### Files

kde_module_LTLIBRARIES = libmswriteimport.la libmswriteexport.la

libmswriteimport_la_SOURCES = mswriteimport.cc \
								libmswrite_defs.cpp structures_generated.cpp structures_private.cpp structures.cpp libmswrite.cpp ImportDialogUI.ui ImportDialog.cc

libmswriteimport_la_LDFLAGS = -module $(KDE_PLUGIN)
libmswriteimport_la_LIBADD = $(KOFFICE_LIBS)

libmswriteexport_la_SOURCES = mswriteexport.cc	\
								libmswrite_defs.cpp structures_generated.cpp structures_private.cpp structures.cpp libmswrite.cpp
libmswriteexport_la_LDFLAGS = -module $(KDE_PLUGIN)
libmswriteexport_la_LIBADD = $(KOFFICE_LIBS) $(LIB_KOWMF) ../libexport/libkwordexportfilters.la

noinst_HEADERS = mswriteimport.h mswriteexport.h \
					  config.libmswrite.h \
					  ImportDialog.h \
					  ImportDialogUI.h \
					  libmswrite_defs.h \
					  libmswrite.h \
					  list.h \
					  structures_generated.h \
					  structures.h \
					  structures_private.h
METASOURCES = AUTO

service_DATA = kword_mswrite_import.desktop kword_mswrite_export.desktop
servicedir = $(kde_servicesdir)
