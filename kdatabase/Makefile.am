\
INCLUDES  = $(KOFFICE_INCLUDES) $(all_includes)

## The part
kde_module_LTLIBRARIES = libkdatabasepart.la
libkdatabasepart_la_SOURCES = kdatabase_part.cc \
	kdatabase_view.cc kdatabase_factory.cc maindlg2.cpp \
        kdatabase_qbuilder.ui qbuilddlg.cpp clstbldesigner2.cpp \
        kdatabase_struct.cpp
libkdatabasepart_la_LDFLAGS = $(KDE_PLUGIN)
libkdatabasepart_la_LIBADD = $(LIB_KOFFICEUI)

## The kdeinit loadable module
lib_LTLIBRARIES = kdatabase.la
kdatabase_la_SOURCES = main.cc
kdatabase_la_LDFLAGS = $(all_libraries) -module -avoid-version
kdatabase_la_LIBADD = $(LIB_KOFFICEUI)

## The executable
bin_PROGRAMS = kdatabase
kdatabase_SOURCES = dummy.cc
kdatabase_LDFLAGS = $(all_libraries) $(KDE_RPATH)
kdatabase_LDADD = kdatabase.la

dummy.cc:
	echo > dummy.cc

METASOURCES = AUTO
noinst_HEADERS =  kdatabase_part.h \
	kdatabase_view.h kdatabase_factory.h kdatabase_aboutdata.h

kdelnk_DATA = kdatabase.desktop
kdelnkdir = $(kde_appsdir)/Office

kdemime_DATA = x-kdatabase.desktop
kdemimedir = $(kde_mimedir)/application

rc_DATA = kdatabase.rc
rcdir = $(kde_datadir)/kdatabase

SUBDIRS = . dtd templates

messages: rc.cpp
	$(XGETTEXT) rc.cpp *.cc kdatabase_aboutdata.h -o $(podir)/kdatabase.pot

