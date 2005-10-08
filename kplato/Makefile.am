SUBDIRS = chart templates pics reports toolbar

INCLUDES = $(KOFFICE_INCLUDES) -I$(top_srcdir)/kdgantt -I$(top_srcdir)/kugar/lib \
           $(all_includes)

libkplatopart_la_SOURCES =   kptnode.cc kptproject.cc \
    kptrelation.cc kptresource.cc kpttask.cc \
    kptduration.cc kptfactory.cc kptpart.cc kptview.cc \
    kptdurationwidget.ui relationpanel.ui\
    kptprojectdialog.cc kpttaskdialog.cc  \
    kptmainprojectdialog.cc kptmainprojectpanel.cc kptmainprojectpanelbase.ui \
    kptganttview.cc kptresourcespanel.cc \
    kptpertview.cc kptpertcanvas.cc kptrelationdialog.cc \
    kptcanvasitem.cc kptprojectdialogbase.ui \
    resourcespanelbase.ui \
    kptresourceview.cc \
    kpttaskresourcespanelbase.ui kptrequestresourcespanel.cc \
    resourcedialogbase.ui kptresourcedialog.cc \
    kpttaskgeneralpanelbase.ui \
    kpttasknotespanelbase.ui \
    kptdatetime.cc kptreportview.cc kpttaskappointmentsview.ui \
    kptcalendar.cc kptcalendaredit.cc kptcalendarlistdialog.cc \
    kptcalendarpanel.cc kptdatetable.cc \
    kptcalendarlistdialogbase.cc kptcalendareditbase.cc \
    kptcommand.cc KPtViewIface.cc KPtViewIface.skel  \
    standardworktimedialogbase.ui kptstandardworktimedialog.cc \
    kptintervaleditbase.ui kptintervaledit.cc \
    kpttaskgeneralpanel.cc kpttaskdefaultpanel.cc \
    kptconfigdialog.cc \
    kpttaskprogresspanel.cc kpttaskprogresspanelbase.ui \
    kptconfigbehaviorpanel.cc kptconfigbehaviorpanelbase.ui \
    kptconfig.cc kptcontext.cc \
    kptresourceuseview.cc kptresourcesdialog.cc \
    kptwbsdefinitiondialog.cc kptwbsdefinitionpanel.cc kptwbsdefinitionpanelbase.ui \
    kptwbsdefinition.cc

# (this is not used)
noinst_HEADERS = kptaboutdata.h kptduration.h kptfactory.h \
    kptnode.h kptpart.h kptproject.h kptrelation.h \
    kptresource.h kpttask.h kptview.h \
    kptprojectdialog.h kpttaskdialog.h  \
    kptmainprojectdialog.h \
    kptganttview.h kptcanvasitem.h \
    kptpertview.h kptpertcanvas.h kptrelationdialog.h \
    kptresourcespanel.h kptresourceview.h \
    kptrequestresourcespanel.h kptresourcedialog.h\
    kptdatetime.h kptreportview.h kptmap.h \
    kptcalendar.h kptcalendaredit.h kptcalendarlistdialog.h \
    kptcalendarpanel.h kptdatetable.h \
    kptcalendarlistdialogbase.h kptcalendareditbase.h \
    kptcommand.h \
    kptstandardworktimedialog.h kptintervaledit.h \
    kpttaskgeneralpanel.h kpttaskdefaultpanel.h \
    kptconfigdialog.h \
    kpttaskprogresspanel.h \
    kptconfigbehaviorpanel.h \
    kptconfig.h kptcontext.h \
    kptresourceuseview.h kptresourcesdialog.h \
    kptwbsdefinitiondialog.h kptwbsdefinitionpanel.h \
    kptwbsdefinition.h
    
## The part
lib_LTLIBRARIES = 
kde_module_LTLIBRARIES = libkplatopart.la
libkplatopart_la_LDFLAGS = $(KDE_LDFLAGS) $(KDE_PLUGIN)
libkplatopart_la_LIBADD = $(LIB_KABC) $(LIB_KOFFICEUI) \
                          $(top_builddir)/kdgantt/libkdgantt.la \
                          $(top_builddir)/kugar/lib/libkugarlib.la \
                          chart/libkptchart.la
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
rc_DATA = kplato.rc kplato_readonly.rc

messages: rc.cpp
	$(XGETTEXT) rc.cpp *.cc kptaboutdata.h -o $(podir)/kplato.pot
kde_services_DATA = kplatopart.desktop
