####### General stuff

INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) -I$(top_srcdir)/kontour -I$(top_srcdir)/lib/kopainter/ $(all_includes)
libxfigimport_la_LDFLAGS	= -module -avoid-version -no-undefined
libxfigimport_la_LIBADD = $(KOFFICE_LIBS) ../../../kontour/libkontourpart.la

####### Files

kde_module_LTLIBRARIES = libxfigimport.la

libxfigimport_la_SOURCES = xfigimport.cc xfigimport_factory.cc

noinst_HEADERS = xfigimport.h xfigimport_factory.h

METASOURCES = AUTO

service_DATA = kontour_xfig_import.desktop
servicedir = $(kde_servicesdir)
