####### General stuff

INCLUDES= $(KOFFICE_INCLUDES) $(all_includes)
librtfimport_la_LDFLAGS	= $(all_libraries) -module -avoid-version -no-undefined
librtfimport_la_LIBADD = $(KOFFICE_LIBS)

####### Files

kde_module_LTLIBRARIES = librtfimport.la

librtfimport_la_SOURCES = rtfimport.cpp rtfimport_dom.cpp rtfimport_tokenizer.cpp

noinst_HEADERS = rtfimport.h rtfimport_dom.h rtfimport_tokenizer.h

METASOURCES = AUTO

service_DATA = kword_rtf_import.desktop
servicedir = $(kde_servicesdir)
