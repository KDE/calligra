####### General stuff

INCLUDES= -I$(srcdir) -I$(srcdir)/../parser -I../parser $(all_includes)
liblatexgenerator_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
liblatexgenerator_la_LIBADD  = -lm $(KOFFICE_LIBS)

####### Files

kde_module_LTLIBRARIES = liblatexgenerator.la

liblatexgenerator_la_SOURCES = document.cc kwordgenerator.cc

noinst_HEADERS = document.h kwordgenerator.h

