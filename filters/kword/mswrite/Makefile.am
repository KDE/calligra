####### General stuff

INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) $(all_includes)

####### Files

kde_module_LTLIBRARIES = libmswriteimport.la

libmswriteimport_la_SOURCES = mswriteimport.cc mswritelib.cc ImportDialogUI.ui ImportDialog.cc
libmswriteimport_la_LDFLAGS = -module $(KDE_PLUGIN)
libmswriteimport_la_LIBADD = $(KOFFICE_LIBS)

noinst_HEADERS = mswriteimport.h mswritelib.h ImportDialog.h
METASOURCES = AUTO

service_DATA = kword_mswrite_import.desktop
servicedir = $(kde_servicesdir)

messages: rc.cpp
	        $(XGETTEXT) *.cc *.cpp -o $(podir)/kwordmswritefilter.pot 
