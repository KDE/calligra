SUBDIRS = templates

INCLUDES = $(KOFFICE_INCLUDES) $(all_includes)

libkplatopart_la_SOURCES =  kptmilestone.cc kptnode.cc kptproject.cc \
	kptrelation.cc kptresource.cc kpttask.cc \
	kptduration.cc kptfactory.cc kptpart.cc kptview.cc \
	kptprojectdialog.cc kpttaskdialog.cc kptnodeitem.cc

noinst_HEADERS = defs.h kptaboutdata.h kptduration.h kptfactory.h \
	kptmilestone.h kptnode.h kptpart.h kptproject.h kptrelation.h \
	kptresource.h kpttask.h kptterminalnode.h kptview.h \
	kptprojectdialog.h kpttaskdialog.h kptnodeitem.h

## The part
kde_module_LTLIBRARIES = libkplatopart.la
libkplatopart_la_LDFLAGS = $(KDE_PLUGIN)
libkplatopart_la_LIBADD = $(LIB_KOFFICEUI)
libkplatopart_la_METASOURCES = AUTO

## The kdeinit loadable module
lib_LTLIBRARIES = kplato.la
kplato_la_SOURCES = main.cc
kplato_la_LDFLAGS = $(all_libraries) -avoid-version -module
kplato_la_LIBADD = $(LIB_KOFFICECORE)

## The executable
bin_PROGRAMS = kplato
kplato_SOURCES = dummy.cc
kplato_LDFLAGS = $(all_libraries) $(KDE_RPATH)
kplato_LDADD = kplato.la

dummy.cc:
	echo > dummy.cc

kdelnk_DATA = kplato.desktop
kdelnkdir = $(kde_appsdir)/Office

kdemime_DATA = x-kplato.desktop
kdemimedir = $(kde_mimedir)/application

rcdir = $(kde_datadir)/kplato
rc_DATA = kplato.rc

messages: rc.cpp
	$(XGETTEXT) rc.cpp *.cc kptaboutdata.h -o $(podir)/kplato.pot
