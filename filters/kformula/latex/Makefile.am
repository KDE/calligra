####### General stuff

INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) $(KFORMULA_INCLUDES) $(all_includes)
libkfmlatexexport_la_LDFLAGS	= $(all_libraries) -module -avoid-version -no-undefined
libkfmlatexexport_la_LIBADD    = $(KOFFICE_LIBS) $(LIB_KFORMULA)

####### Files

kde_module_LTLIBRARIES = libkfmlatexexport.la

libkfmlatexexport_la_SOURCES = latexexport.cc

noinst_HEADERS = latexexport.h

######## Debug
#check_PROGRAMS = texlauncher
#texlauncher_SOURCES = texlauncher.cc
#texlauncher_LDADD = liblatexexport.la
#texlauncher_LDFLAGS = $(KDE_RPATH) $(all_libraries)


METASOURCES = AUTO

service_DATA = kformula_latex_export.desktop
servicedir = $(kde_servicesdir)

messages: rc.cpp
	$(XGETTEXT) *.cc *.cpp -o $(podir)/kformulalatexfilter.pot
