####### General stuff

INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) $(KFORMULA_INCLUDES) $(all_includes)
libkfosvgexport_la_LDFLAGS	= $(all_libraries) -module -avoid-version -no-undefined
libkfosvgexport_la_LIBADD    = $(KOFFICE_LIBS) $(LIB_KFORMULA)

####### Files

kde_module_LTLIBRARIES = libkfosvgexport.la

libkfosvgexport_la_SOURCES = svgexport.cc

noinst_HEADERS = svgexport.h

######## Debug
#check_PROGRAMS = texlauncher
#texlauncher_SOURCES = texlauncher.cc
#texlauncher_LDADD = liblatexexport.la
#texlauncher_LDFLAGS = $(KDE_RPATH) $(all_libraries)


METASOURCES = AUTO

service_DATA = kformula_svg_export.desktop
servicedir = $(kde_servicesdir)
