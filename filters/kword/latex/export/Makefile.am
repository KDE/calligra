####### General stuff

INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) $(KFORMULA_INCLUDES) $(LIBMAGICK_CPPFLAGS) $(all_includes)
libkwordlatexexport_la_LDFLAGS	= $(all_libraries) -module -avoid-version -no-undefined $(LIBMAGICK_RPATH) $(LIBMAGICK_LDFLAGS)
libkwordlatexexport_la_LIBADD    = $(KOFFICE_LIBS) $(LIB_KFORMULA) $(LIBMAGICK_LIBS)

####### Files

kde_module_LTLIBRARIES = libkwordlatexexport.la

libkwordlatexexport_la_SOURCES = anchor.cc config.cc document.cc element.cc fileheader.cc \
																 latexexportdia.ui footnote.cc format.cc formula.cc key.cc \
																 kwordlatexexportdia.cc latexexport.cc latexexportIface.cc \
																 latexexportIface.skel layout.cc listtable.cc para.cc \
																 pixmapFrame.cc table.cc \
																 textFrame.cc textformat.cc textzone.cc variableformat.cc \
																 variablezone.cc xml2latexparser.cc xmlparser.cc

noinst_HEADERS = anchor.h border.h config.h document.h element.h fileheader.h footnote.h \
								 format.h formula.h key.h kwordlatexexportdia.h latexexport.h latexexportdia.h \
								 latexexportIface.h layout.h \
								 listtable.h para.h pixmapFrame.h table.h textFrame.h textformat.h \
								 textzone.h variableformat.h variablezone.h xml2latexparser.h xmlparser.h

######## Debug
#check_PROGRAMS = texlauncher
#texlauncher_SOURCES = texlauncher.cc
#texlauncher_LDADD = liblatexexport.la
#texlauncher_LDFLAGS = $(KDE_RPATH) $(all_libraries)


METASOURCES = AUTO

service_DATA = kword_latex_export.desktop
servicedir = $(kde_servicesdir)

messages: rc.cpp
	$(XGETTEXT) *.cc *.cpp -o $(podir)/kwordlatexexportfilter.pot
