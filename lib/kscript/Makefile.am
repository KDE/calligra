
KDE_CXXFLAGS = $(USE_RTTI)
INCLUDES = $(all_includes)
AM_LDFLAGS   = $(all_libraries)

####### Files

lib_LTLIBRARIES = libkscript.la
libkscript_la_SOURCES = kscript_proxy.cc kscript_eval.cc kscript_object.cc kscript_class.cc kscript_func.cc \
		  kscript_context.cc kscript_value.cc kscript_parser.cc kscript_parsenode.cc yacc.cc \
                  scanner.cc kscript.cc kscript_util.cc kscript_struct.cc kscript_scriptmenu.cc \
                  kscript_ext_qstructs.cc kscript_ext_qt.cc kscript_ext_qwidget.cc \
		  kscript_ext_qdialog.cc kscript_ext_qapplication.cc kscript_ext_qlineedit.cc \
		  kscript_ext_qbutton.cc kscript_ext_qcheckbox.cc kscript_ext_qradiobutton.cc \
		  kscript_ext_qpushbutton.cc kscript_ext_qcombobox.cc kscript_ext_qlabel.cc \
		  kscript_ext_qboxlayout.cc kscript_ext_qvboxlayout.cc kscript_ext_qhboxlayout.cc \
		  kscript_ext_qbuttongroup.cc kscript_ext_qlistbox.cc

libkscript_la_METASOURCES = AUTO

libkscript_la_LDFLAGS = -version-info 1:0:0 -no-undefined

libkscript_la_LIBADD = $(LIB_KDEUI)

bin_PROGRAMS = kscript

kscript_SOURCES = main.cc
noinst_HEADERS = kscript_eval.h kscript_object.h kscript_class.h kscript_func.h kscript_context.h kscript_value.h \
		 kscript_parser.h kscript_parsenode.h kscript.h kscript_util.h kscript_struct.h kscript_proxy.h \
		 kscript_ptr.h kscript_synext.h kscript_types.h kscript_qt.h kscript_qstructs.h

kscript_LDADD = ./libkscript.la
kscript_LDFLAGS = $(KDE_RPATH)

# SUBDIRS = ksidl

####### Implicit rules

####### Build rules

parser:
	cd $(srcdir) && flex -B -8 -oscanner.cc scanner.ll  ;\
	bison -d -t -v -o yacc.cc yacc.yy

