####### General stuff

INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) $(KFORMULA_INCLUDES) $(all_includes)
libkspreadlatexexport_la_LDFLAGS	= $(all_libraries) -module -avoid-version -no-undefined
libkspreadlatexexport_la_LIBADD    = $(KOFFICE_LIBS) $(LIB_KFORMULA)

####### Files

kde_module_LTLIBRARIES = libkspreadlatexexport.la

libkspreadlatexexport_la_SOURCES = cell.cc column.cc config.cc document.cc fileheader.cc \
				 format.cc latexexportdia.ui kspreadlatexexportdiaImpl.cc \
				 latexexport.cc xmlparser.cc \
                                 latexexportdia.skel map.cc pen.cc spreadsheet.cc table.cc

noinst_HEADERS = cell.h column.h config.h document.h fileheader.h format.h \
					kspreadlatexexportdiaImpl.h latexexport.h \
					map.h pen.h spreadsheet.h table.h xmlparser.h

######## Debug
#check_PROGRAMS = texlauncher
#texlauncher_SOURCES = texlauncher.cc
#texlauncher_LDADD = liblatexexport.la
#texlauncher_LDFLAGS = $(KDE_RPATH) $(all_libraries)

METASOURCES = AUTO

service_DATA = kspread_latex_export.desktop
servicedir = $(kde_servicesdir)

messages: rc.cpp
	$(XGETTEXT) *.cc *.cpp -o $(podir)/kspreadlatexexportfilter.pot
