
KDE_CXXFLAGS = $(USE_RTTI)
INCLUDES = $(all_includes)
LDFLAGS   = $(all_libraries)

####### Files

lib_LTLIBRARIES = libkscript.la
libkscript_la_SOURCES = kscript_proxy.cc kscript_eval.cc kscript_object.cc kscript_class.cc kscript_func.cc kscript_context.cc \
		  kscript_value.cc kscript_parser.cc kscript_parsenode.cc yacc.cc scanner.cc kscript.cc \
		  kscript_util.cc kscript_struct.cc kscript_scriptmenu.cc kscript_qt.cc kscript_qstructs.cc \
		  kscript_qobject.cc

libkscript_la_METASOURCES = kscript_scriptmenu.moc

libkscript_la_LDFLAGS = -version-info 1:0:0 -no-undefined

libkscript_la_LIBADD = $(LIB_KDEUI)

bin_PROGRAMS = kscript

kscript_SOURCES = main.cc
noinst_HEADERS = kscript_eval.h kscript_object.h kscript_class.h kscript_func.h kscript_context.h kscript_value.h \
		 kscript_parser.h kscript_parsenode.h kscript.h kscript_util.h kscript_struct.h kscript_proxy.h \
		 kscript_ptr.h kscript_synext.h kscript_types.h kscript_qt.h kscript_qstructs.h

kscript_LDADD = ./libkscript.la
kscript_LDFLAGS = $(KDE_RPATH)

DISTCLEANFILES = $(idl_DATA:.idl=.cc) $(idl_HEADERS) $(SRCMETA)

# SUBDIRS = ksidl

####### Implicit rules

####### Build rules

parser:
	cd $(srcdir) && flex -B -8 -oscanner.cc scanner.ll  ;\
	bison -d -t -v -o yacc.cc yacc.yy

