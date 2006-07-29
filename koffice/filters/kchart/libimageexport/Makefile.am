####### General stuff

INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) \
        -I$(top_srcdir)/kchart \
        -I$(top_srcdir)/kchart/kdchart \
        -I$(top_srcdir)/interfaces \
        -I$(top_srcdir)/filters/libdialogfilter \
        $(all_includes)

####### Files

lib_LTLIBRARIES = libkchartimageexport.la

libkchartimageexport_la_SOURCES = imageexport.cpp
libkchartimageexport_la_LDFLAGS = $(all_libraries) -version-info 4:0 -no-undefined
libkchartimageexport_la_LIBADD = ../../../kchart/libkchartcommon.la ../../../filters/libdialogfilter/libdialogfilter.la $(KOFFICE_LIBS)

noinst_HEADERS = \
	imageexport.h

METASOURCES = AUTO


