####### General stuff

INCLUDES= \
	-I$(srcdir) \
	$(KWMF_INCLUDES) \
	$(KOFFICE_INCLUDES) \
	$(KOPAINTER_INCLUDES) \
	$(all_includes)

libwmfimport_la_LDFLAGS	= -module -avoid-version -no-undefined $(all_libraries)
libwmfimport_la_LIBADD = $(KOFFICE_LIBS) $(LIB_KWMF)

####### Files

kde_module_LTLIBRARIES = libwmfimport.la

libwmfimport_la_SOURCES = wmfimport.cc

noinst_HEADERS = wmfimport.h

METASOURCES = AUTO

service_DATA = karbon_wmf_import.desktop
servicedir = $(kde_servicesdir)
