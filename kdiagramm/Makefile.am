
SUBDIRS = pics

INCLUDES  = $(MICO_INCLUDES) $(KOFFICE_INCLUDES) -I$(interfacedir) $(all_includes) 
LDFLAGS   = $(all_libraries) $(MICO_LDFLAGS) $(KDE_RPATH)
LDADD = ../lib/kofficeui/libkofficeui.la 

####### Files

bin_PROGRAMS = kdiagramm

kdiagramm_SOURCES = kdiagramm.idl kdiagramm_main.cc kdiagramm_shell.cc \
			kdiagramm_doc.cc kdiagramm_view.cc koDiagramm.cc chart.idl \
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

noinst_HEADERS =	kdiagramm_main.h kdiagramm_shell.h kdiagramm_doc.h \
					kdiagramm.h kdiagramm_view.h chart.h \
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

METASOURCES = kdiagramm_main.moc kdiagramm_shell.moc kdiagramm_doc.moc \
	kdiagramm_view.moc koDiagramm.moc table.moc sheet.moc sheetdlg.moc \
	KoDiagrammColorConfigPage.moc KoDiagrammBarConfigDialog.moc \
	KoDiagrammAreaConfigDialog.moc KoDiagrammPieConfigDialog.moc \
	KoDiagrammLinesConfigDialog.moc KoDiagrammLinesPointsConfigDialog.moc \
	KoDiagrammPointsConfigDialog.moc KoDiagrammConfigDialog.moc \
	KoDiagrammBarConfigPage.moc KoDiagrammGeometryConfigPage.moc

noinst_HEADERS = kdiagramm_main.h kdiagramm_shell.h kdiagramm_doc.h \
	         kdiagramm.h kdiagramm_view.h chart.h \
		 sheet.h table.h parser.h sheetdlg.h

idl_DATA = kdiagramm.idl
kdiagramm.h : $(top_builddir)/lib/kofficecore/koffice.idl

kdelnk_DATA = kdiagramm.desktop
kdelnkdir = $(kde_appsdir)/KOffice

####### Implicit rules

####### Build rules

IDL_FLAGS = --relative-paths --any -I$(top_srcdir)/lib/kofficecore -I$(top_srcdir)/lib/store -I$(top_srcdir)/lib/interfaces -I$(idldir)

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot *.cc && mv messages.po ../po/kdiagramm.pot

