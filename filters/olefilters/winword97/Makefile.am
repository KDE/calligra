####### General stuff

INCLUDES = $(KOFFICE_INCLUDES) -I$(srcdir) -I$(srcdir)/../lib $(all_includes)

####### Files

noinst_LTLIBRARIES = libwinword97.la

libwinword97_la_SOURCES = wordfilter.cc winworddoc.cc \
msdrawing.cc \
msword.cc mswordgenerated.cc properties.cc document.cc
libwinword97_la_LDFLAGS = $(all_libraries)

noinst_HEADERS = wordfilter.h winworddoc.h \
msdrawing.h \
msword.h mswordgenerated.h properties.h document.h

METASOURCES = AUTO

install-data-local: uninstall.desktop
	$(INSTALL_DATA) $(srcdir)/uninstall.desktop $(DESTDIR)$(kde_servicesdir)/ole_winword97_import.desktop

#service_DATA = ole_winword97_import.desktop
#servicedir = $(kde_servicesdir)
