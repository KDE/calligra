####### General stuff

INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) $(all_includes)

####### Files

kde_module_LTLIBRARIES = libstarwriterimport.la

libstarwriterimport_la_SOURCES = starwriterimport.cc koledevice.cc kolestorage.cc kolestorageinfo.cc
libstarwriterimport_la_LDFLAGS	= -module $(KDE_PLUGIN)
libstarwriterimport_la_LIBADD = $(KOFFICE_LIBS)

METASOURCES = AUTO

service_DATA = kword_starwriter_import.desktop
servicedir = $(kde_servicesdir)
