###### General stuff

KDE_CXXFLAGS = $(USE_RTTI)
INCLUDES=	-I$(top_srcdir)/lib/kofficecore -I$(top_builddir)/lib/kofficecore $(all_includes)
LDFLAGS	=	$(all_libraries) -version-info 1:0:0
libkofficeui_la_LIBADD  = ../kofficecore/libkofficecore.la $(LIB_KDEUI)

####### Files

SUBDIRS = . pics test

lib_LTLIBRARIES = libkofficeui.la

libkofficeui_la_SOURCES = koPageLayoutDia.cc \
	koPartSelectDia.cc \
	koRuler.cc koTemplateChooseDia.cc koTabChooser.cc \
	koKoolBar.cc koUIUtils.cc koTemplateCreateDia.cc

include_HEADERS = koPageLayoutDia.h \
	koPartSelectDia.h \
	koRuler.h koTemplateChooseDia.h koTabChooser.h \
	koKoolBar.h koUIUtils.h koTemplateCreateDia.h

METASOURCES = AUTO

####### Implicit rules



