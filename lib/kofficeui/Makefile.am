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
	kcoloractions.cpp koTemplates.cc \
	koRect.cpp koPictureFilePreview.cc koUnitWidgets.cc \
	tkaction.cpp tkcoloractions.cpp tkcombobox.cpp tktoolbarbutton.cpp \
	koCharSelectDia.cc koInsertLink.cc koeditpath.cc kocommandhistory.cpp \
	koselectaction.cpp kolinewidthaction.cpp kolinestyleaction.cpp \
	kotoolbutton.cc koImageResource.cc kotoolbox.cc kozoomhandler.cpp \
    koGuideLines.cpp koGuides.cpp

libkofficeui_la_LDFLAGS	= -version-info 3:0:0 -no-undefined $(all_libraries)
libkofficeui_la_LIBADD  = $(LIB_KOFFICECORE) 

include_HEADERS = \
	koPageLayoutDia.h kozoomaction.h kotabbar.h \
	koPartSelectDia.h koPartSelectAction.h \
	koRuler.h koTemplateChooseDia.h koTabChooser.h \
	koKoolBar.h koTemplateCreateDia.h kocontexthelp.h \
	kcoloractions.h \
	koRect.h koPoint.h koSize.h koPictureFilePreview.h koUnitWidgets.h \
	tkaction.h tkcoloractions.h tktoolbarbutton.h tkcombobox.h \
	koCharSelectDia.h koInsertLink.h kotoolbutton.h koeditpath.h \
	kocommandhistory.h  koImageResource.h \
	koselectaction.h kolinewidthaction.h kolinestyleaction.h kozoomhandler.h \
    koGuideLines.h koGuides.h

# FIXME: Disabled for now as it breaks the installation process if
# KOffice is not installed in $KDEDIR
# 
#AM_CXXFLAGS = -DQT_PLUGIN
#kde_widget_LTLIBRARIES = kofficewidgets.la
#kofficewidgets_la_LDFLAGS = $(KDE_PLUGIN) -module $(all_libraries)
#kofficewidgets_la_LIBADD = $(LIB_KIO) libkofficeui.la
#kofficewidgets_la_SOURCES =  kofficewidgets.cpp

#kofficewidgets.cpp: $(srcdir)/koffice.widgets
#	$(MAKEKDEWIDGETS) -o kofficewidgets.cpp $(srcdir)/koffice.widgets

#CLEANFILES = kofficewidgets.cpp

METASOURCES = AUTO
