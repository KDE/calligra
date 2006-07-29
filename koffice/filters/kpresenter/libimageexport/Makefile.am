####### General stuff

INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) \
	-I$(top_srcdir)/kpresenter \
        -I$(top_srcdir)/lib/kotext \
	$(all_includes)

####### Files

lib_LTLIBRARIES = libkpresenterimageexport.la

libkpresenterimageexport_la_SOURCES = imageexport.cpp
libkpresenterimageexport_la_LDFLAGS = $(all_libraries) -version-info 4:0 -no-undefined
libkpresenterimageexport_la_LIBADD = ../../../kpresenter/libkpresenterprivate.la $(KOFFICE_LIBS)
noinst_HEADERS = \
	imageexport.h

METASOURCES = AUTO


