INCLUDES  = $(KOFFICE_INCLUDES) -I$(srcdir)/kdchart -I$(top_srcdir)/interfaces -I$(interfacedir) $(all_includes)

## The part
lib_LTLIBRARIES = 
kde_module_LTLIBRARIES = libkchartpart.la
libkchartpart_la_SOURCES = kchart_part.cc \
	kchart_view.cc kchart_factory.cc \
	kchartBarConfigDialog.cc \
	kchartBarConfigPage.cc \
	kchartColorConfigPage.cc \
	kchartParameterConfigPage.cc \
	kchartParameter3dConfigPage.cc \
	kchartBackgroundPixmapConfigPage.cc	\
	kchartSubTypeChartPage.cc	\
	kchartComboConfigPage.cc \
	kchartFontConfigPage.cc \
	kchartDataConfigPage.cc \
	kchartPieConfigPage.cc \
	kchartParameterPieConfigPage.cc \
	kchartConfigDialog.cc \
	kchartGeometryConfigPage.cc \
	kchartWizard.cc \
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
	table.cc qttableview.cpp KChartViewIface.skel KChartViewIface.cc \
	kchartLegendConfigPage.cc kchart_params.cc KChartParamsIface.skel \
	KChartParamsIface.cc kchartHeaderFooterConfigPage.cc \
	kchartLine3dConfigPage.cc kchartParameterPolarConfigPage.cc kchartPageLayout.cc \
	kchartPrinterDlg.cc

libkchartpart_la_LDFLAGS = $(KDE_PLUGIN)
libkchartpart_la_LIBADD = $(top_builddir)/interfaces/libkochart.la \
                          $(LIB_KOFFICEUI) \
			  kdchart/libkdchart.la			

## The kdeinit loadable module and executable
kdeinit_LTLIBRARIES = kchart.la
bin_PROGRAMS =
kchart_la_SOURCES = main.cc
kchart_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
kchart_la_LIBADD = $(LIB_KOFFICECORE)

noinst_HEADERS =  kchart_part.h \
		kchart_view.h kchart_factory.h \
		kchartBarConfigDialog.h \
		kchartBarConfigPage.h \
		kchartColorConfigPage.h \
		kchartParameterConfigPage.h \
		kchartParameter3dConfigPage.h \
		kchartBackgroundPixmapConfigPage.h	\
		kchartSubTypeChartPage.h	\
		kchartComboConfigPage.h \
		kchartParameterPieConfigPage.h \
		kchartFontConfigPage.h \
		kchartDataConfigPage.h \
		kchartPieConfigPage.h \
		kchartConfigDialog.h \
		kchartGeometryConfigPage.h \
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
		table.h qttableview.h \
		kchart_aboutdata.h	\
		kchartcolorarray.h KChartViewIface.h \
		kchartLegendConfigPage.h  kchart_params.h \
		KChartParamsIface.h kchartHeaderFooterConfigPage.h \
		kchartLine3dConfigPage.h kchartParameterPolarConfigPage.h \
		kchartPageLayout.h \
		kchartPrinterDlg.h

METASOURCES = AUTO

rcdir = $(kde_datadir)/kchart
rc_DATA = kchart.rc kchart_readonly.rc

kdelnk_DATA = kchart.desktop
kdelnkdir = $(kde_appsdir)/Office

# SUBDIRS =  . plugins pics dtd toolbar
SUBDIRS =  kdchart . toolbar pics

messages: rc.cpp
	$(XGETTEXT) rc.cpp *.cc kchart_aboutdata.h kdchart/*.cpp -o $(podir)/kchart.pot
kde_services_DATA = kchartpart.desktop
