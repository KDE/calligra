
INCLUDES = $(all_includes)

bin_PROGRAMS = kthesaurus

kthesaurus_LDFLAGS = $(all_libraries) $(KDE_RPATH)
kthesaurus_SOURCES = main.cc
kthesaurus_LDADD = $(LIB_KSYCOCA)
 
appsdir = $(kde_appsdir)/Office
apps_DATA = KThesaurus.desktop

messages:
	$(XGETTEXT) *.cc -o $(podir)/kthesaurus.pot
