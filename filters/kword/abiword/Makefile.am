####### General stuff

INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) $(all_includes)
libabiwordimport_la_LDFLAGS	= -module -avoid-version -no-undefined
libabiwordimport_la_LIBADD = $(KOFFICE_LIBS)
libabiwordexport_la_LDFLAGS	= -module -avoid-version -no-undefined
libabiwordexport_la_LIBADD = $(KOFFICE_LIBS)

####### Files

lib_LTLIBRARIES = libabiwordimport.la libabiwordexport.la

libabiwordimport_la_SOURCES = abiwordimport.cc abiwordimport_factory.cc
libabiwordexport_la_SOURCES = abiwordexport.cc abiwordexport_factory.cc

noinst_HEADERS = abiwordimport.h abiwordimport_factory.h abiwordexport.h \
abiwordexport_factory.h

METASOURCES = AUTO

service_DATA = kword_abiword_import.desktop kword_abiword_export.desktop
servicedir = $(kde_servicesdir)

kdemime_DATA = x-abiword.desktop
kdemimedir = $(kde_mimedir)/application
