lib_LTLIBRARIES = kexitablefilters.la

SUBDIRS = .  csv

kexitablefilters_la_SOURCES = kexitablefiltermanager.cpp kexitablefilter.cpp kexitableimportfilter.cpp
kexitablefilters_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) -module
kexitablefilters_la_LIBADD = ../../kexiDB/libkexidb.la

KDE_CXXFLAGS = $(USE_EXCEPTIONS)

INCLUDES= $(KOFFICE_INCLUDES) -I$(top_srcdir)/kexi/core -I$(top_srcdir)/kexi \
		-I$(top_srcdir)/kexi/widget -I$(top_srcdir)/kexi/tableview \
		-I$(top_srcdir)/kexi/kexiDB $(all_includes)

METASOURCES = AUTO

