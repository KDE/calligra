
KDE_CXXFLAGS = $(USE_RTTI)
INCLUDES = $(all_includes)
LDFLAGS   = $(all_libraries)

####### Files

lib_LTLIBRARIES = libkoscript.la
libkoscript_la_SOURCES = koscript_locale.cc \
		  koscript_eval.cc koscript_func.cc koscript_property.cc koscript_method.cc \
		  koscript_context.cc koscript_value.cc koscript_parser.cc koscript_parsenode.cc yacc.cc \
                  scanner.cc koscript.cc koscript_util.cc koscript_struct.cc

libkoscript_la_METASOURCES = AUTO

libkoscript_la_LDFLAGS = -version-info 1:0:0 -no-undefined

libkoscript_la_LIBADD = $(LIB_KDEUI)

bin_PROGRAMS = koscript

koscript_SOURCES = main.cc
noinst_HEADERS = koscript_locale.h \
		 koscript_eval.h koscript_func.h koscript_context.h koscript_value.h \
		 koscript_parser.h koscript_parsenode.h koscript.h koscript_util.h koscript_struct.h \
		 koscript_synext.h koscript_types.h koscript_method.h

koscript_LDADD = ./libkoscript.la
koscript_LDFLAGS = $(KDE_RPATH)

DISTCLEANFILES = $(idl_DATA:.idl=.cc) $(idl_HEADERS) $(SRCMETA)

####### Implicit rules

####### Build rules

parser:
	cd $(srcdir) && flex -B -8 -oscanner.cc scanner.ll  ;\
	bison -d -t -v -o yacc.cc yacc.yy

