# $Id$

####### General stuff

INCLUDES= $(all_includes) $(MICO_INCLUDES)
CXXFLAGS += -O -DINCLUDE_MOC_BULLSHIT

####### Files

lib_LTLIBRARIES = libkchartcore.la
libkchartcore_la_SOURCES = \
	KChart.cpp KChartWidget.cpp KChartPainter.cpp KChartAxesPainter.cpp \
	KChartAreaPainter.cpp KChartBarsPainter.cpp KChartPointsPainter.cpp \
	KChartLinesPainter.cpp KChartLinesPointsPainter.cpp KChartPiePainter.cpp \
	KChartColorArray.cpp KChartData.cpp KChartPie3DPainter.cpp 

libkchartcore_la_LIBADD = -lkdecore -lqt -lXext -lX11
libkchartcore_la_LDFLAGS = $(all_libraries) -version-info 1:0:0
include_HEADERS = KChart.h KChartTypes.h KChartColorArray.h KChartData.h KChartWidget.h

SUFFIXES = .h .moc

.h.moc:
	$(MOC) $< -o $@

DISTCLEANFILES = $(BUILT_SOURCES)

noinst_PROGRAMS = test test2 test3 test4 test5 test6 test7 test10 test11 test14
noinst_HEADERS = KChartAreaPainter.h KChartAxesPainter.h KChartBarsPainter.h \
	KChartLinesPainter.h KChartLinesPointsPainter.h KChartPointsPainter.h \
	KChartPiePainter.h KChartPie3DPainter.h 

BUILT_SOURCES = KChart.moc KChartData.moc KChartWidget.moc

KChart.cpp: KChart.moc
KChartData.cpp: KChartData.moc
KChartWidget.cpp: KChartWidget.moc

test_SOURCES = main.cpp
test2_SOURCES = main2.cpp
test3_SOURCES = main3.cpp
test4_SOURCES = main4.cpp
test5_SOURCES = main5.cpp
test6_SOURCES = main6.cpp
test7_SOURCES = main7.cpp
test10_SOURCES = main10.cpp
test11_SOURCES = main11.cpp
test12_SOURCES = main12.cpp
test13_SOURCES = main13.cpp
test14_SOURCES = main14.cpp

LDFLAGS = $(all_libraries)
LDADD = libkchartcore.la -lkdecore -lqt -lXext -lX11 $(LIBSOCKET)

