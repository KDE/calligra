
SUBDIRS = pics

INCLUDES  = $(MICO_INCLUDES) $(KOFFICE_INCLUDES) -I$(interfacedir) $(all_includes) 
LDFLAGS   = $(all_libraries) $(MICO_LDFLAGS) $(KDE_RPATH)
LDADD = ../lib/kofficeui/libkofficeui.la 

####### Files

bin_PROGRAMS = kchart

kchart_SOURCES = chart.idl kchart.idl kchart_main.cc kchart_shell.cc \
			kchart_doc.cc kchart_view.cc koChart.cc \
			sheet.cc table.cc parser.cc sheetdlg.cc KChartAreaPainter.cpp \
			KChartAxesPainter.cpp KChartBarsPainter.cpp KChartLinesPainter.cpp\
			KChartLinesPointsPainter.cpp KChartPainter.cpp \
			KChartPie3DPainter.cpp KChartPiePainter.cpp \
			KChartPointsPainter.cpp KoChartColorConfigPage.cpp \
			KoChartBarConfigDialog.cpp KoChartAreaConfigDialog.cpp \
			KoChartPieConfigDialog.cpp KoChartLinesConfigDialog.cpp \
			KoChartLinesPointsConfigDialog.cpp \
			KoChartPointsConfigDialog.cpp KoChartConfigDialog.cpp \
			KoChartBarConfigPage.cpp KoChartGeometryConfigPage.cpp \
			KChartWizard.cpp \
			KChartWizardLabelsLegendPage.cpp \
			KChartWizardSelectChartSubTypePage.cpp \
			KChartWizardSelectChartTypePage.cpp \
			KChartWizardSelectDataPage.cpp \
			KChartWizardSetupAxesPage.cpp \
			KChartWizardSetupDataPage.cpp 


noinst_HEADERS =	kchart_main.h kchart_shell.h kchart_doc.h \
					kchart.h kchart_view.h chart.h \
					sheet.h table.h parser.h sheetdlg.h KChartAreaPainter.h \
					KChartAxesPainter.h KChartBarsPainter.h \
					KChartLinesPainter.h KChartLinesPointsPainter.h \
					KChartPainter.h	KChartPie3DPainter.h \
					KChartPiePainter.h KChartPointsPainter.h \
					KChartColorConfigPage.h \
					KChartBarConfigDialog.h \
					KChartPieConfigDialog.h \
					KChartAreaConfigDialog.h \
					KChartLinesConfigDialog.h \
					KChartLinesPointsConfigDialog.h \
					KChartPointsConfigDialog.h \
					KChartConfigDialog.h \
					KChartBarConfigPage.h \
					KChartGeometryConfigPage.h \
					KChartWizard.h \
					KChartWizardLabelsLegendPage.h \
					KChartWizardSelectChartSubTypePage.h \
					KChartWizardSelectChartTypePage.h \
					KChartWizardSelectDataPage.h \
					KChartWizardSetupAxesPage.h \
					KChartWizardSetupDataPage.h 



chart_IDLDIR = $(interfacedir)

METASOURCES = kchart_main.moc kchart_shell.moc kchart_doc.moc \
	kchart_view.moc koChart.moc table.moc sheet.moc sheetdlg.moc \
	KoChartColorConfigPage.moc KoChartBarConfigDialog.moc \
	KoChartAreaConfigDialog.moc KoChartPieConfigDialog.moc \
	KoChartLinesConfigDialog.moc KoChartLinesPointsConfigDialog.moc \
	KoChartPointsConfigDialog.moc KoChartConfigDialog.moc \
	KoChartBarConfigPage.moc KoChartGeometryConfigPage.moc \
	KChartWizard.moc \
	KChartWizardLabelsLegendPage.moc \
	KChartWizardSelectChartSubTypePage.moc \
	KChartWizardSelectChartTypePage.moc \
	KChartWizardSelectDataPage.moc \
	KChartWizardSetupAxesPage.moc \
	KChartWizardSetupDataPage.moc 



noinst_HEADERS = kchart_main.h kchart_shell.h kchart_doc.h \
	         kchart.h kchart_view.h chart.h \
		 sheet.h table.h parser.h sheetdlg.h

idl_DATA = kchart.idl
kchart.h : $(top_builddir)/lib/kofficecore/koffice.idl

kdelnk_DATA = kchart.desktop
kdelnkdir = $(kde_appsdir)/KOffice

####### Implicit rules

####### Build rules

IDL_FLAGS = --relative-paths --any -I$(top_srcdir)/lib/kofficecore -I$(top_srcdir)/lib/store -I$(top_srcdir)/lib/interfaces -I$(idldir)

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot *.cc && mv messages.po ../po/kchart.pot

