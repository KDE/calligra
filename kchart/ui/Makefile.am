# $Id$

####### General stuff

INCLUDES= -I$(srcdir)/../core $(all_includes) $(MICO_INCLUDES)

####### Files

lib_LTLIBRARIES = libkchartui.la
libkchartui_la_SOURCES = \
	KChartWizard.cpp KChartWizardLabelsLegendPage.cpp \
	KChartWizardSelectChartSubTypePage.cpp KChartWizardSelectChartTypePage.cpp\
	KChartWizardSelectDataPage.cpp KChartWizardSetupAxesPage.cpp \
	KChartWizardSetupDataPage.cpp

libkchartui_la_LIBADD  = $(LIB_KDECORE)
libkchartui_la_LDFLAGS = $(all_libraries) -version-info 1:0:0
include_HEADERS = KChartWizard.h

noinst_PROGRAMS = test
noinst_HEADERS = KChartWizardLabelsLegendPage.h KChartWizardSelectDataPage.h \
	KChartWizardSelectChartSubTypePage.h KChartWizardSelectChartTypePage.h \
	KChartWizardSetupAxesPage.h KChartWizardSetupDataPage.h main.h

METASOURCES = KChartWizardSelectChartTypePage.moc \
		KChartWizardLabelsLegendPage.moc \
		KChartWizardSetupAxesPage.moc \
		KChartWizard.moc \
		KChartWizardSelectChartSubTypePage.moc \
		KChartWizardSetupDataPage.moc main.moc

test_SOURCES = main.cpp 

LDFLAGS = $(all_libraries)
LDADD = libkchartui.la ../core/libkchartcore.la $(LIB_KDEUI)

KDE_ICON = lo-chart_area lo-chart_bars lo-chart_barsinfront \
	lo-chart_barsontop lo-chart_lines lo-chart_linespoints \
	lo-chart_normalbars lo-chart_pie lo-chart_pie3D \
	lo-chart_points


