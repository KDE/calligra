####### General stuff

INCLUDES = $(KOFFICE_INCLUDES) $(all_includes)
librtfimport_la_LDFLAGS	= -module $(KDE_PLUGIN)
librtfimport_la_LIBADD = $(KOFFICE_LIBS)

####### Files

kde_module_LTLIBRARIES = librtfimport.la

librtfimport_la_SOURCES = KRTFTokenizer.cpp KRTFParser.cpp KRTFFileParser.cpp \
                KRTFHeaderParser.cpp KRTFDocumentParser.cpp

#### Use test_PROGRAMS (and "make check") to create a test program using main.cpp

METASOURCES = AUTO

service_DATA = kword_rtf_import.desktop
servicedir = $(kde_servicesdir)

