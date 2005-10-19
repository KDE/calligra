####### General stuff

INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) \
	-I$(top_srcdir)/kpresenter \
        -I$(top_srcdir)/lib/kotext \
	$(all_includes)

####### Files

kde_module_LTLIBRARIES = libimageexport.la

libimageexport_la_SOURCES = imageexport.cpp
libimageexport_la_LDFLAGS = -module $(KDE_PLUGIN) -no-undefined
libimageexport_la_LIBADD = ../../../kpresenter/libkpresenterpart.la $(KOFFICE_LIBS)
noinst_HEADERS = \
	imageexport.h

METASOURCES = AUTO


