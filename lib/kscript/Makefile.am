
INCLUDES = $(MICO_INCLUDES) $(all_includes)
LDFLAGS   = $(all_libraries) $(MICO_LDFLAGS)

####### Files

lib_LTLIBRARIES = libkscript.la
libkscript_la_SOURCES = kscript_proxy.cc kscript_eval.cc kscript_object.cc kscript_class.cc kscript_func.cc kscript_context.cc \
		  kscript_value.cc kscript_parser.cc kscript_parsenode.cc yacc.cc scanner.cc kscript.cc \
		  kscript_util.cc kscript_ext_qwidget.cc kscript_ext_qpushbutton.cc kscript_ext_qradiobutton.cc \
		  kscript_ext_qlabel.cc kscript_ext_qapplication.cc kscript_ext_qt.cc kscript_ext_qlineedit.cc \
		  kscript_ext_qcheckbox.cc kscript_ext_qrect.cc kscript_struct.cc kscript_interface.cc \
		  kscript_typecode.cc kscript_corbafunc.cc kscript_corba.cc kscript_ext_qvboxlayout.cc

libkscript_la_METASOURCES = kscript_ext_qt.moc

libkscript_la_LDFLAGS = -version-info 1:0:0

libkscript_la_LIBADD = $(LIB_QT) $(LIBMICO)

bin_PROGRAMS = kscript

kscript_SOURCES = main.cc
noinst_HEADERS = kscript_eval.h kscript_object.h kscript_class.h kscript_func.h kscript_context.h kscript_value.h \
		 kscript_parser.h kscript_parsenode.h kscript.h kscript_util.h \
		 kscript_ext_qwidget.h kscript_ext_qpushbutton.h kscript_ext_qradiobutton.h kscript_ext_qlabel.h \
		 kscript_ext_qapplication.h kscript_ext_qt.h kscript_ext_qlineedit.h \
		 kscript_ext_qcheckbox.h kscript_ext_qrect.cc kscript_struct.h kscript_interface.h kscript_proxy.h \
		 kscript_typecode.h kscript_corbafunc.h kscript_corba.h kscript_ptr.h kscript_ext_qvboxlayout.h

kscript_LDADD = ./libkscript.la
kscript_LDFLAGS = $(KDE_RPATH)

DISTCLEANFILES = $(idl_DATA:.idl=.cc) $(idl_HEADERS) $(SRCMETA)

SUBDIRS = ksidl

####### Implicit rules

####### Build rules

parser:
	cd $(srcdir) && flex -B -8 -oscanner.cc scanner.ll  ;\
	bison -d -t -v -o yacc.cc yacc.yy

