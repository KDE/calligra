
SUBDIRS = pics

INCLUDES  = $(PYTHONINC) $(MICO_INCLUDES) $(KOFFICE_INCLUDES) -I$(interfacedir) $(all_includes) -I$(idldir)
LDFLAGS   = $(all_libraries) $(PYTHONLIB) $(MICO_LDFLAGS) $(KDE_RPATH)
LDADD = ../lib/kofficeui/libkofficeui.la 

####### Files

bin_PROGRAMS = kdiagramm.bin

kdiagramm_bin_SOURCES = kdiagramm_main.cc kdiagramm_shell.cc kdiagramm_doc.cc \
			kdiagramm.cc kdiagramm_view.cc koDiagramm.cc chart.cc \
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
idl_HEADERS = kdiagramm.h

DISTCLEANFILES = $(idl_DATA:.idl=.cc) $(idl_HEADERS) chart.h chart.cc

kdelnk_DATA = kdiagramm_part.kdelnk
kdelnkdir = $(kde_datadir)/koffice/partlnk

####### Implicit rules

####### Build rules

IDL_FLAGS = --query-server-for-narrow --relative-paths -I$(top_srcdir)/lib/kofficecore -I$(top_srcdir)/lib/store -I$(top_srcdir)/lib/interfaces -I$(idldir)

install-exec-local:
	$(INSTALL_SCRIPT) $(srcdir)/kdiagramm $(bindir)/kdiagramm

uninstall-local:
	-rm -f $(bindir)/kdiagramm

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot *.cc && mv messages.po ../po/kdiagramm.pot

kdiagramm.h: kdiagramm.idl
	$(IDL) $(IDL_FLAGS) $(srcdir)/kdiagramm.idl

chart.h: $(interfacedir)/chart.idl
	$(IDL) $(IDL_FLAGS) $(interfacedir)/chart.idl

BUILT_SOURCES = kdiagramm.h chart.h

