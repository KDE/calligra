kde_module_LTLIBRARIES = libkarbonaiimport.la

libkarbonaiimport_la_LIBADD = \
	../../../../koffice/lib/kofficeui/libkofficeui.la

libkarbonaiimport_la_LDFLAGS = \
	$(all_libraries) \
	$(KDE_RPATH) \
	-version-info 1:0:0 -no-undefined

INCLUDES = \
	-I$(srcdir)/.. \
	-I$(srcdir)/../../../../koffice/lib/kofficecore \
	-I$(srcdir)/../../../../koffice/lib/kofficeui \
	$(all_includes)

service_DATA = karbon_ai_import.desktop
servicedir = $(kde_servicesdir)

noinst_HEADERS = \
	aidocument.h \
	aiimport.h \
	yacc.cc.h

libkarbonaiimport_la_SOURCES = \
	aidocument.cc \
	aiimport.cc \
	scanner.cc \
	yacc.cc

libkarbonaiimport_la_METASOURCES = \
	AUTO

parser:
	cd $(srcdir) && flex -B -8 -oscanner.cc scanner.ll ; \
	bison -t -d -v -o yacc.cc yacc.yy

EXTRA_DIST = scanner.ll yacc.yy

MAINTAINERCLEANFILES = yacc.cc.output

