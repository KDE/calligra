####### General stuff

INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) $(KFORMULA_INCLUDES) $(all_includes)
libkfopngexport_la_LDFLAGS	= $(all_libraries) -module -avoid-version -no-undefined
libkfopngexport_la_LIBADD    = $(KOFFICE_LIBS) $(LIB_KFORMULA)

####### Files

kde_module_LTLIBRARIES = libkfopngexport.la

libkfopngexport_la_SOURCES = pngexport.cc pngexportdia.cc

noinst_HEADERS = pngexport.h pngexportdia.h

######## Debug
#check_PROGRAMS = texlauncher
#texlauncher_SOURCES = texlauncher.cc
#texlauncher_LDADD = liblatexexport.la
#texlauncher_LDFLAGS = $(KDE_RPATH) $(all_libraries)


METASOURCES = AUTO

service_DATA = kformula_png_export.desktop
servicedir = $(kde_servicesdir)
