####### General stuff

INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) $(KFORMULA_INCLUDES) $(LIBMAGICK_CPPFLAGS) $(all_includes)
libkwordlatexexport_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined $(LIBMAGICK_RPATH) $(LIBMAGICK_LDFLAGS)
libkwordlatexexport_la_LIBADD  = $(KOFFICE_LIBS) $(LIB_KFORMULA) $(LIBMAGICK_LIBS)

####### Files

kde_module_LTLIBRARIES = libkwordlatexexport.la

libkwordlatexexport_la_SOURCES = anchor.cc config.cc document.cc element.cc fileheader.cc \
 latexexportdia.ui footnote.cc format.cc formula.cc key.cc \
 kwordlatexexportdia.cc latexexport.cc latexexportIface.cc \
 latexexportIface.skel layout.cc listtable.cc para.cc \
 pixmapFrame.cc table.cc \
 textFrame.cc textformat.cc textzone.cc variableformat.cc \
 variablezone.cc xml2latexparser.cc xmlparser.cc

######## Debug
#check_PROGRAMS = texlauncher
#texlauncher_SOURCES = texlauncher.cc
#texlauncher_LDADD = liblatexexport.la
#texlauncher_LDFLAGS = $(KDE_RPATH) $(all_libraries)

METASOURCES = AUTO

service_DATA = kword_latex_export.desktop
servicedir = $(kde_servicesdir)
