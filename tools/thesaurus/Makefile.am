
INCLUDES = $(all_includes)
libthesaurustool_la_LIBADD = $(LIB_KSYCOCA)

####### Files

kde_module_LTLIBRARIES = libthesaurustool.la

libthesaurustool_la_SOURCES = main.cc

libthesaurustool_la_LDFLAGS = $(all_libraries) -avoid-version -module -no-undefined

noinst_HEADERS = main.h

libthesaurustool_la_METASOURCES = main.moc

service_DATA = thesaurustool.desktop
servicedir = $(kde_servicesdir)

thesaurusdir = $(kde_datadir)/thesaurus
thesaurus_DATA = thesaurus.txt

messages:
	$(XGETTEXT) *.cc -o $(podir)/thesaurus_tool.pot
