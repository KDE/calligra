###### General stuff

KDE_CXXFLAGS = $(USE_RTTI)
INCLUDES =  $(KSTORE_INCLUDES) $(KOFFICECORE_INCLUDES) $(all_includes)

####### Files

SUBDIRS = . tests pics

lib_LTLIBRARIES = libkofficeui.la

libkofficeui_la_SOURCES = \
        koPageLayoutDia.cc kozoomaction.cpp kotabbar.cpp \
	koPartSelectDia.cc koPartSelectAction.cc \
	koRuler.cc koTemplateChooseDia.cc koTabChooser.cc \
	koKoolBar.cc koTemplateCreateDia.cc kocontexthelp.cc\
	kcoloractions.cpp koToolBox.cpp koTemplates.cc \
	koRect.cpp koPictureFilePreview.cc koUnitWidgets.cc \
	tkaction.cpp tkcoloractions.cpp tkcombobox.cpp tktoolbarbutton.cpp \
	koCharSelectDia.cc koInsertLink.cc koeditpath.cc kocommandhistory.cpp \
	koselectaction.cpp kolinewidthaction.cpp kolinestyleaction.cpp \
	kotooldockbase.cpp kotooldockmanager.cpp kotooldockmovemanager.cpp \
	kotoolbutton.cc

libkofficeui_la_LDFLAGS	= -version-info 2:0:0 -no-undefined $(all_libraries)
libkofficeui_la_LIBADD  = $(LIB_KOFFICECORE) 

include_HEADERS = \
	koPageLayoutDia.h kozoomaction.h kotabbar.h \
	koPartSelectDia.h koPartSelectAction.h \
	koRuler.h koTemplateChooseDia.h koTabChooser.h \
	koKoolBar.h koTemplateCreateDia.h kocontexthelp.h \
	kcoloractions.h koToolBox.h \
	koRect.h koPoint.h koSize.h koPictureFilePreview.h koUnitWidgets.h \
	tkaction.h tkcoloractions.h tktoolbarbutton.h tkcombobox.h \
	koCharSelectDia.h koInsertLink.h kotoolbutton.h koeditpath.h \
	kocommandhistory.h \
	koselectaction.h kolinewidthaction.h kolinestyleaction.h \
	kotooldockbase.h kotooldockmanager.h kotooldockmovemanager.h

METASOURCES = AUTO
