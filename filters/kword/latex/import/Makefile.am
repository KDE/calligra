SUBDIRS = parser

####### General stuff

INCLUDES= -I$(srcdir) -I$(srcdir)/parser $(KOFFICE_INCLUDES) $(KFORMULA_INCLUDES) $(all_includes)
liblateximport_la_LDFLAGS= -module -avoid-version -no-undefined
liblateximport_la_LIBADD = $(KOFFICE_LIBS) parser/liblatexparser.la

####### Files

kde_module_LTLIBRARIES = liblateximport.la

liblateximport_la_SOURCES = config.cc lateximport.cc lateximportdia.cc lateximportdia.skel

noinst_HEADERS = config.h lateximport.h lateximportdia.h

######## Debug
#check_PROGRAMS = texlauncher
#texlauncher_SOURCES = texlauncher.cc
#texlauncher_LDADD = liblatexexport.la
#texlauncher_LDFLAGS = $(KDE_RPATH) $(all_libraries)


METASOURCES = AUTO

service_DATA = kword_latex_import.desktop
servicedir = $(kde_servicesdir)

messages: rc.cpp
	$(XGETTEXT) *.cc *.cpp -o $(podir)/kwordlatexfilter.pot
