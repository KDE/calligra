INCLUDES  = $(KOFFICE_INCLUDES) -I$(interfacedir) $(all_includes)

kchart_LDFLAGS = $(all_libraries) $(KDE_RPATH)
kchart_LDADD = ../lib/kofficeui/libkofficeui.la

libkchart_la_LDFLAGS = -avoid-version -module

libkchart_la_LIBADD = ../lib/kofficeui/libkofficeui.la




####### Files

bin_PROGRAMS = kchart
lib_LTLIBRARIES = libkchart.la

libkchart_la_SOURCES = kchart_shell.cc kchart_part.cc \
	kchart_view.cc kchart_factory.cc \
	engine.cc \
	enginehelper.cc \
	enginedraw.cc \
	enginepie.cc \
	enginetoremove.cc \
	kchartparams.cc \
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



kchart_SOURCES = main.cc

noinst_HEADERS =  kchart_global.h \
		  kchart_shell.h kchart_part.h \
		  kchart_view.h kchart_factory.h	\
	          engine.h \
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


libkchart_la_METASOURCES = kchart_shell.moc kchart_part.moc \
			       kchart_view.moc kchart_factory.moc \
		kchartWizard.moc \
		kchartWizardLabelsLegendPage.moc \
		kchartWizardSelectChartSubTypePage.moc \
		kchartWizardSelectChartTypePage.moc\
		kchartWizardSetupAxesPage.moc \
	        kchartWizardSetupDataPage.moc \
		kchartDataEditor.moc \
		sheet.moc \
		sheetdlg.moc \
		table.moc

rcdir = $(kde_datadir)/kchart
rc_DATA = kchart.rc kchart_shell.rc




kdelnk_DATA = kchart.desktop
kdelnkdir = $(kde_appsdir)/Applications/KOffice

kdemime_DATA = x-kchart.desktop
kdemimedir = $(kde_mimedir)/application

# linktest - from dfaure 

check_PROGRAMS = linktest
linktest_SOURCES = $(libkchart_la_SOURCES)
linktest_LDADD = $(lib_LTLIBRARIES)
linktest_LDFLAGS = $(libkchart_LDFLAGS) 



# SUBDIRS =  . plugins pics dtd toolbar

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot *.cc && mv messages.po ../po/kchart.pot


