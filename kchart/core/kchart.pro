TEMPLATE    =	app
CONFIG      =	qt warn_on debug

HEADERS     =				\
		KChart.h 			\
		KChartData.h		\
		KChartColorArray.h 	\
		KChartWidget.h		\
		KChartPainter.h		\
		KChartAxesPainter.h	\
		KChartAreaPainter.h	\
		KChartBarsPainter.h	\
		KChartLinesPainter.h	\
		KChartPointsPainter.h	\
		KChartLinesPointsPainter.h	\
		KChartPiePainter.h			\
		KChartPie3DPainter.h		\
		KChartTypes.h

SOURCES     =	\
		KChart.cpp 			\
		KChartData.cpp		\
		KChartWidget.cpp	\
		KChartPainter.cpp	\
		KChartAxesPainter.cpp	\
		KChartAreaPainter.cpp	\
		KChartBarsPainter.cpp	\
		KChartLinesPainter.cpp	\
		KChartPointsPainter.cpp	\
		KChartLinesPointsPainter.cpp	\
		KChartPiePainter.cpp		\
		KChartPie3DPainter.cpp	\
		KChartColorArray.cpp	\
		main12.cpp

TARGET      =	kchart

INCLUDEPATH = /usr/local/KDE/include

LIBS = -L/usr/local/KDE/lib -lkdecore -lXext -lg++