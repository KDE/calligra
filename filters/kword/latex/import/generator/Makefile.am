####### General stuff

INCLUDES= -I$(srcdir) -I$(srcdir)/../parser -I../parser $(all_includes)
liblatexgenerator_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
liblatexgenerator_la_LIBADD  = -lm $(KOFFICE_LIBS)

####### Files

kde_module_LTLIBRARIES = liblatexgenerator.la

liblatexgenerator_la_SOURCES = body.cc document.cc kwordgenerator.cc latex.cc paragraph.cc

noinst_HEADERS = body.h document.h kwordgenerator.h latex.h paragraph.h

