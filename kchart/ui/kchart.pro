TEMPLATE    =	app
CONFIG      =	qt warn_on debug

HEADERS     =				\
		KChartWizard.h 			\
		KChartWizardSelectDataPage.h		\
		KChartWizardSelectChartTypePage.h	\
		KChartWizardSetupChartPage.h		\
		KChartWizardSetupAxesPage.h

SOURCES     =	\
		KChartWizard.cpp 			\
		KChartWizardSelectDataPage.cpp		\
		KChartWizardSelectChartTypePage.cpp		\
		KChartWizardSetupChartPage.cpp			\
		KChartWizardSetupAxesPage.cpp			\
		main.cpp

TARGET      =	kchart

INCLUDEPATH = /usr/local/KDE/include

LIBS = -L/usr/local/KDE/lib -lkchartcore -lkdeui -lkdecore -lXext -lg++