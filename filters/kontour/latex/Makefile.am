#SUBDIRS=launcher
####### General stuff

INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) $(all_includes)
#libtexgraphimport_la_LDFLAGS	= -module -avoid-version -no-undefined
#libtexgraphimport_la_LIBADD = $(KOFFICE_LIBS) $(LIB_KWMF)
libtexgraphexport_la_LDFLAGS	= -module -avoid-version -no-undefined
libtexgraphexport_la_LIBADD = $(KOFFICE_LIBS)

####### Files

kde_module_LTLIBRARIES = libtexgraphexport.la 
#libtexgraphimport.la

#libtexgraphimport_la_SOURCES = 

####### Files

libtexgraphexport_la_SOURCES = bezier.cc document.cc element.cc ellipse.cc \
font.cc group.cc header.cc layer.cc \
page.cc point.cc polygone.cc polyline.cc \
texgraphexport.cc texgraphexport_factory.cc rectangle.cc \
text.cc xmlparser.cc

 noinst_HEADERS = bezier.h document.h element.h ellipse.h \
font.h group.h header.h layer.h \
page.h point.h polygone.h polyline.h \
texgraphexport.h texgraphexport_factory.h rectangle.h \
text.h xmlparser.h

######## Debug
check_PROGRAMS = texlauncher
texlauncher_SOURCES = texlauncher.cc
texlauncher_LDADD = libtexgraphexport.la
texlauncher_LDFLAGS = $(KDE_RPATH) $(all_libraries)




METASOURCES = AUTO

service_DATA = kontour_latex_export.desktop
servicedir = $(kde_servicesdir)
#data_DATA = x-latex.desktop
datadir = $(kde_mimedir)/image
