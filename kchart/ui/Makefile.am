# $Id$

####### General stuff

INCLUDES= -I$(srcdir)/../core $(all_includes) $(MICO_INCLUDES)
CXXFLAGS = $(CXXFLAGS) -O -DINCLUDE_MOC_BULLSHIT

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
	KChartWizardSetupAxesPage.h KChartWizardSetupDataPage.h

METASOURCES = KChartWizardSelectChartTypePage.moc \
		KChartWizardLabelsLegendPage.moc \
		KChartWizard.moc \
		KChartWizardSelectChartSubTypePage.moc \
		KChartWizardSetupDataPage.moc 

$(srcdir)/main.cpp: main.moc
main.moc: main.cpp
	$(MOC) $(srcdir)/main.cpp -o main.moc

test_SOURCES = main.cpp 

LDFLAGS = $(all_libraries)
LDADD = libkchartui.la ../core/libkchartcore.la $(LIB_KDEUI)

picsdir = $(kde_toolbardir)
pics_DATA = chart_area.gif chart_bars.gif chart_barsinfront.gif \
	chart_barsontop.gif chart_lines.gif chart_linespoints.gif \
	chart_normalbars.gif chart_pie.gif chart_pie3D.gif \
	chart_points.gif


