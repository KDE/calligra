####### General stuff

INCLUDES= -I$(srcdir) $(all_includes)
liblatexparser_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
liblatexparser_la_LIBADD  = -lfl -lm $(KOFFICE_LIBS)

####### Files

kde_module_LTLIBRARIES = liblatexparser.la

liblatexparser_la_SOURCES = command.cc comment.cc element.cc env.cc latexparser.cc param.cc text.cc

noinst_HEADERS = command.h comment.h element.h env.h latexparser.h param.h option.h text.h

######## Debug
#check_PROGRAMS = texlauncher
#texlauncher_SOURCES = texlauncher.cc
#texlauncher_LDADD = liblatexexport.la
#texlauncher_LDFLAGS = $(KDE_RPATH) $(all_libraries)


METASOURCES = AUTO

