SUBDIRS = templates pics reports

INCLUDES = $(KOFFICE_INCLUDES) -I$(top_srcdir)/kdgantt -I$(top_srcdir)/kugar/lib \
                   $(all_includes)

libkplatopart_la_SOURCES =   kptnode.cc kptproject.cc \
    kptrelation.cc kptresource.cc kpttask.cc \
    kptduration.cc kptfactory.cc kptpart.cc kptview.cc \
    kptprojectdialog.cc kpttaskdialog.cc  \
    kptganttview.cc kptresourcespanel.cc \
    kptpertview.cc kptpertcanvas.cc kptrelationdialog.cc \
    kptcanvasitem.cc kptprojectdialogbase.ui resourcespanelbase.ui \
    durationpanelbase.ui kptdurationdialog.cc \
    kptresourceview.cc \
    requestresourcespanelbase.ui kptrequestresourcespanel.cc \
    kpttaskdialogbase.ui resourcedialogbase.ui kptresourcedialog.cc \
    kptdatetime.cc kptreportview.cc kpttaskappointmentsview.ui

# (this is not used)
noinst_HEADERS = defs.h kptaboutdata.h kptduration.h kptfactory.h \
    kptnode.h kptpart.h kptproject.h kptrelation.h \
    kptresource.h kpttask.h kptterminalnode.h kptview.h \
    kptprojectdialog.h kpttaskdialog.h  \
    kptganttview.h kptcanvasitem.h \
    kptpertview.h kptpertcanvas.h kptrelationdialog.h \
    kptresourcespanel.h kptdurationdialog.h kptresourceview.h \
    kptrequestresourcespanel.h kptresourcedialog.h\
    kptdatetime.h kptreportview.h

## The part
lib_LTLIBRARIES = 
kde_module_LTLIBRARIES = libkplatopart.la
libkplatopart_la_LDFLAGS = $(KDE_LDFLAGS) $(KDE_PLUGIN)
libkplatopart_la_LIBADD = $(LIB_KABC) $(LIB_KOFFICEUI) $(top_builddir)/kdgantt/libkdgantt.la $(top_builddir)/kugar/lib/libkugar.la
libkplatopart_la_METASOURCES = AUTO

## The kdeinit loadable module and executable
kdeinit_LTLIBRARIES = kplato.la
bin_PROGRAMS =
kplato_la_SOURCES = main.cc
kplato_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
kplato_la_LIBADD = $(LIB_KOFFICECORE)

kdelnk_DATA = kplato.desktop
kdelnkdir = $(kde_appsdir)/Office

rcdir = $(kde_datadir)/kplato
rc_DATA = kplato.rc

messages: rc.cpp
	$(XGETTEXT) rc.cpp *.cc kptaboutdata.h -o $(podir)/kplato.pot
kde_services_DATA = kplatopart.desktop
