
INCLUDES = $(KOFFICECORE_INCLUDES) $(all_includes)

bin_PROGRAMS =
lib_LTLIBRARIES =
kdeinit_LTLIBRARIES = kthesaurus.la

kthesaurus_la_LDFLAGS = $(all_libraries) $(KDE_RPATH) -module -avoid-version
kthesaurus_la_SOURCES = main.cc
kthesaurus_la_LIBADD = $(LIB_KSYCOCA)
 
appsdir = $(kde_appsdir)/Office
apps_DATA = KThesaurus.desktop

messages:
	$(XGETTEXT) *.cc -o $(podir)/kthesaurus.pot
