####### General stuff

INCLUDES= -I$(srcdir) -I$(top_srcdir)/killustrator $(KOFFICE_INCLUDES) $(all_includes)

####### Files

kde_module_LTLIBRARIES = libsvgexport.la

#libsvgimport_la_SOURCES = svgimport.cc svgimport_factory.cc
#libsvgimport_la_LDFLAGS	= -module -avoid-version -no-undefined
#libsvgimport_la_LIBADD  = $(KOFFICE_LIBS) ../../../killustrator/libkillustratorpart.la

libsvgexport_la_SOURCES = svgexport.cc svgexport_factory.cc
libsvgexport_la_LDFLAGS	= -module -avoid-version -no-undefined
libsvgexport_la_LIBADD  = $(KOFFICE_LIBS) ../../../killustrator/libkillustratorpart.la

METASOURCES = AUTO

service_DATA = killustrator_svg_export.desktop
servicedir = $(kde_servicesdir)

data_DATA = x-svg.desktop
datadir = $(kde_mimedir)/image
