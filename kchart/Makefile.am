
SUBDIRS = pics

INCLUDES  = $(MICO_INCLUDES) $(KOFFICE_INCLUDES) -I$(interfacedir) $(all_includes) 
LDFLAGS   = $(all_libraries) $(MICO_LDFLAGS) $(KDE_RPATH)
LDADD = ../lib/kofficeui/libkofficeui.la 

####### Files

bin_PROGRAMS = kchart

kchart_SOURCES = kchart.idl kchart_main.cc kchart_shell.cc \
			kchart_doc.cc kchart_view.cc koDiagramm.cc chart.idl \
			sheet.cc table.cc parser.cc sheetdlg.cc KChartAreaPainter.cpp \
			KChartAxesPainter.cpp KChartBarsPainter.cpp KChartLinesPainter.cpp\
			KChartLinesPointsPainter.cpp KChartPainter.cpp \
			KChartPie3DPainter.cpp KChartPiePainter.cpp \
			KChartPointsPainter.cpp KoDiagrammColorConfigPage.cpp \
			KoDiagrammBarConfigDialog.cpp KoDiagrammAreaConfigDialog.cpp \
			KoDiagrammPieConfigDialog.cpp KoDiagrammLinesConfigDialog.cpp \
			KoDiagrammLinesPointsConfigDialog.cpp \
			KoDiagrammPointsConfigDialog.cpp KoDiagrammConfigDialog.cpp \
			KoDiagrammBarConfigPage.cpp KoDiagrammGeometryConfigPage.cpp

noinst_HEADERS =	kchart_main.h kchart_shell.h kchart_doc.h \
					kchart.h kchart_view.h chart.h \
					sheet.h table.h parser.h sheetdlg.h KChartAreaPainter.h \
					KChartAxesPainter.h KChartBarsPainter.h \
					KChartLinesPainter.h KChartLinesPointsPainter.h \
					KChartPainter.h	KChartPie3DPainter.h \
					KChartPiePainter.h KChartPointsPainter.h \
					KoDiagrammColorConfigPage.h \
					KoDiagrammBarConfigDialog.h \
					KoDiagrammPieConfigDialog.h \
					KoDiagrammAreaConfigDialog.h \
					KoDiagrammLinesConfigDialog.h \
					KoDiagrammLinesPointsConfigDialog.h \
					KoDiagrammPointsConfigDialog.h \
					KoDiagrammConfigDialog.h \
					KoDiagrammBarConfigPage.h \
					KoDiagrammGeometryConfigPage.h


chart_IDLDIR = $(interfacedir)

METASOURCES = kchart_main.moc kchart_shell.moc kchart_doc.moc \
	kchart_view.moc koDiagramm.moc table.moc sheet.moc sheetdlg.moc \
	KoDiagrammColorConfigPage.moc KoDiagrammBarConfigDialog.moc \
	KoDiagrammAreaConfigDialog.moc KoDiagrammPieConfigDialog.moc \
	KoDiagrammLinesConfigDialog.moc KoDiagrammLinesPointsConfigDialog.moc \
	KoDiagrammPointsConfigDialog.moc KoDiagrammConfigDialog.moc \
	KoDiagrammBarConfigPage.moc KoDiagrammGeometryConfigPage.moc

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

