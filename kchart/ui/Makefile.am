# $Id$

####### General stuff

INCLUDES= $(all_includes) $(MICO_INCLUDES)
CXXFLAGS += -O -DINCLUDE_MOC_BULLSHIT

####### Files

lib_LTLIBRARIES = libkchartui.la
libkchartui_la_SOURCES = \
	KChartWizard.cpp KChartWizardLabelsLegendPage.cpp \
	KChartWizardSelectChartSubTypePage.cpp KChartWizardSelectChartTypePage.cpp\
	KChartWizardSelectDataPage.cpp KChartWizardSetupAxesPage.cpp \
	KChartWizardSetupDataPage.cpp

libkchartui_la_LIBADD = -lkchartcore -lkdecore -lqt -lXext -lX11
libkchartui_la_LDFLAGS = $(all_libraries) -version-info 1:0:0
include_HEADERS = KChartWizard.h

SUFFIXES = .h .moc

.h.moc:
	$(MOC) $< -o $@

DISTCLEANFILES = $(BUILT_SOURCES)

noinst_PROGRAMS = test
noinst_HEADERS = KChartWizardLabelsLegendPage.h KChartWizardSelectDataPage.h \
	KChartWizardSelectChartSubTypePage.h KChartWizardSelectChartTypePage.h \
	KChartWizardSetupAxesPage.h KChartWizardSetupDataPage.h

BUILT_SOURCES = KChartWizardSelectChartTypePage.moc \
				KChartWizardLabelsLegendPage.moc \
				KChartWizard.moc \
				KChartWizardSelectChartSubTypePage.moc \
				KChartWizardSetupDataPage.moc

KChartWizardSelectChartTypePage.cpp: KChartWizardSelectChartTypePage.moc
KChartWizardLabelsLegendPage.cpp: KChartWizardLabelsLegendPage.moc
KChartWizard.cpp: KChartWizard.moc
KChartWizardSelectChartSubTypePage.cpp: KChartWizardSelectChartSubTypePage.moc
KChartWizardSetupDataPage.cpp: KChartWizardSetupDataPage.moc

test_SOURCES = main.cpp

LDFLAGS = $(all_libraries)
LDADD = libkchartui.la -lkchartcore -lkdeui -lkdecore -lqt -lXext -lX11 $(LIBSOCKET)

picsdir = $(kde_toolbardir)
pics_DATA = chart_area.gif chart_bars.gif chart_barsinfront.gif \
			chart_barsontop.gif chart_lines.gif chart_linespoints.gif \
			chart_normalbars.gif chart_pie.gif chart_pie3D.gif \
			chart_points.gif


