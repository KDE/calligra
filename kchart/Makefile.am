INCLUDES  = $(KOFFICE_INCLUDES) -I$(top_srcdir)/interfaces -I$(interfacedir) $(all_includes)

lib_LTLIBRARIES = libkchartpart.la kchart.la
## The part
libkchartpart_la_SOURCES = kchart_part.cc \
	kchart_view.cc kchart_factory.cc \
	engine.cc \
	enginehelper.cc \
	enginedraw.cc \
	enginepie.cc \
	enginetoremove.cc \
	kchartparams.cc \
	kchartBarConfigDialog.cc \
	kchartBarConfigPage.cc \
	kchartColorConfigPage.cc \
	kchartParameterConfigPage.cc \
	kchartParameter3dConfigPage.cc \
	kchartAreaSubTypeChartPage.cc	\
	kchartBackgroundPixmapConfigPage.cc	\
	kchartBarSubTypeChartPage.cc \
	kchartSubTypeChartPage.cc	\
	kchartComboConfigPage.cc \
	kchartFontConfigPage.cc \
	kchartPieConfigPage.cc \
	kchartParameterPieConfigPage.cc \
	kchartConfigDialog.cc \
	kchartGeometryConfigPage.cc \
	kchartWizard.cc \
	kchartEngine.cc \
	kchartEngine2.cc \
	kchartEngine_DataDraw.cc \
	kchartEngine_Grids.cc \
	kchartEngine_ComputeSize.cc \
	kchartWizardLabelsLegendPage.cc \
	kchartWizardSelectChartSubTypePage.cc \
        kchartWizardSelectChartTypePage.cc \
        kchartWizardSelectDataPage.cc \
        kchartWizardSetupAxesPage.cc \
        kchartWizardSetupDataPage.cc \
	kchartDataEditor.cc \
	parser.cc \
	sheet.cc \
	sheetdlg.cc \
	table.cc
libkchartpart_la_LDFLAGS = $(KDE_PLUGIN)
libkchartpart_la_LIBADD = $(top_builddir)/interfaces/libkochart.la \
                          $(top_builddir)/lib/kofficeui/libkofficeui.la

## The kdeinit loadable module
kchart_la_SOURCES = main.cc
kchart_la_LDFLAGS = $(all_libraries) -avoid-version -module
kchart_la_LIBADD = libkchartpart.la

## The executable
bin_PROGRAMS = kchart
kchart_SOURCES = dummy.cc
kchart_LDFLAGS = $(all_libraries) $(KDE_RPATH)
kchart_LDADD = kchart.la

dummy.cc:
	echo > dummy.cc

noinst_HEADERS =  kchart_part.h \
		  kchart_view.h kchart_factory.h	\
	          engine.h \
		kchartBarConfigDialog.h \
		kchartBarConfigPage.h \
		kchartColorConfigPage.h \
		kchartParameterConfigPage.h \
		kchartParameter3dConfigPage.h \
		kchartAreaSubTypeChartPage.h	\
		kchartBackgroundPixmapConfigPage.h	\
		kchartBarSubTypeChartPage.h \
		kchartSubTypeChartPage.h	\
		kchartComboConfigPage.h \
		kchartParameterPieConfigPage.h \
		kchartFontConfigPage.h \
		kchartPieConfigPage.h \
		kchartConfigDialog.h \
		kchartGeometryConfigPage.h \
	   	kchartEngine.h \
		kchartWizard.h \
		kchartWizardLabelsLegendPage.h \
		kchartWizardSelectChartSubTypePage.h \
		kchartWizardSelectChartTypePage.h \
	        kchartWizardSelectDataPage.h \
		kchartWizardSetupAxesPage.h \
	        kchartWizardSetupDataPage.h \
		kchartDataEditor.h \
		parser.h \
		sheet.h \
		sheetdlg.h \
		table.h


METASOURCES = AUTO

rcdir = $(kde_datadir)/kchart
rc_DATA = kchart.rc

kdelnk_DATA = kchart.desktop
kdelnkdir = $(kde_appsdir)/Office

kdemime_DATA = x-kchart.desktop
kdemimedir = $(kde_mimedir)/application

# linktest - from dfaure

check_PROGRAMS = linktest
linktest_SOURCES = $(libkchartpart_la_SOURCES)
linktest_LDADD = $(lib_LTLIBRARIES)
linktest_LDFLAGS = $(libkchartpart_la_LDFLAGS)

# SUBDIRS =  . plugins pics dtd toolbar
SUBDIRS =  . toolbar pics

messages: rc.cpp
	$(XGETTEXT) rc.cpp *.cc -o $(podir)/kchart.pot
