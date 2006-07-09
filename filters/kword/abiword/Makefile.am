####### General stuff

INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) -I$(srcdir)/../libexport $(all_includes)

libabiwordimport_la_LDFLAGS	= $(all_libraries) -module -avoid-version -no-undefined
libabiwordimport_la_LIBADD = $(KOFFICE_LIBS)

libabiwordexport_la_LDFLAGS	= $(all_libraries) -module -avoid-version -no-undefined
libabiwordexport_la_LIBADD = $(KOFFICE_LIBS) ../libexport/libkwordexportfilters.la

####### Files

kde_module_LTLIBRARIES = libabiwordimport.la libabiwordexport.la

libabiwordimport_la_SOURCES = abiwordimport.cc ImportHelpers.cc\
ImportFormatting.cc ImportStyle.cc ImportField.cc

libabiwordexport_la_SOURCES = abiwordexport.cc

noinst_HEADERS = abiwordimport.h ImportHelpers.h ImportFormatting.h\
ImportStyle.h ImportField.h abiwordexport.h

METASOURCES = AUTO

service_DATA = kword_abiword_import.desktop kword_abiword_export.desktop
servicedir = $(kde_servicesdir)
