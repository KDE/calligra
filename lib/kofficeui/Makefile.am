###### General stuff

KDE_CXXFLAGS = $(USE_RTTI)
INCLUDES = $(KOFFICECORE_INCLUDES) $(all_includes)
libkofficeui_la_LDFLAGS	= -version-info 1:0:0 -no-undefined
libkofficeui_la_LIBADD  = $(LIB_KOFFICECORE)

####### Files

SUBDIRS = . pics

lib_LTLIBRARIES = libkofficeui.la

libkofficeui_la_SOURCES = koGlobal.cc koFind.cc koReplace.cc \
        koPageLayoutDia.cc \
	koPartSelectDia.cc koPartSelectAction.cc \
	koRuler.cc koTemplateChooseDia.cc koTabChooser.cc \
	koKoolBar.cc koTemplateCreateDia.cc \
	kcoloractions.cpp koToolBox.cpp koTemplates.cc \
	koRect.cpp \
	tkaction.cpp tkcoloractions.cpp tkcombobox.cpp tktoolbarbutton.cpp

include_HEADERS = koFind.h koReplace.h \
        koPageLayoutDia.h \
	koPartSelectDia.h koPartSelectAction.h \
	koRuler.h koTemplateChooseDia.h koTabChooser.h \
	koKoolBar.h koTemplateCreateDia.h \
	kcoloractions.h koToolBox.h koTemplates.h \
	koGlobal.h tkaction.h tkcoloractions.h tktoolbarbutton.h tkcombobox.h

METASOURCES = AUTO

