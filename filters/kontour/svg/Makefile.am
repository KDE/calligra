####### General stuff

INCLUDES= -I$(srcdir) -I$(top_srcdir)/kontour -I$(top_srcdir)/lib/kopainter/ $(KOFFICE_INCLUDES) $(all_includes)

####### Files

kde_module_LTLIBRARIES = libsvgimport.la libsvgexport.la

libsvgimport_la_SOURCES = svgimport.cc svgimport_factory.cc basicshapes.cc datatypes.cc path.cc transform.cc style.cc text.cc svgmain.cc
libsvgimport_la_LDFLAGS	= -module -avoid-version -no-undefined
libsvgimport_la_LIBADD  = $(KOFFICE_LIBS) ../../../kontour/libkontourpart.la

libsvgexport_la_SOURCES = svgexport.cc svgexport_factory.cc
libsvgexport_la_LDFLAGS	= -module -avoid-version -no-undefined
libsvgexport_la_LIBADD  = $(KOFFICE_LIBS) ../../../kontour/libkontourpart.la

METASOURCES = AUTO

service_DATA = kontour_svg_import.desktop kontour_svg_export.desktop
servicedir = $(kde_servicesdir)
