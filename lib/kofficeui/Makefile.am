####### General stuff

INCLUDES=	-I$(top_srcdir)/lib/kofficecore -I$(top_builddir)/lib/kofficecore $(all_includes) $(MICO_INCLUDES) 
LDFLAGS	=	$(all_libraries) -version-info 1:0:0
libkofficeui_la_LIBADD  = ../kofficecore/libkofficecore.la $(LIB_KDEUI)

####### Files

SUBDIRS = . pics test

lib_LTLIBRARIES = libkofficeui.la

libkofficeui_la_SOURCES = koAboutDia.cc koPageLayoutDia.cc \
	koPartSelectDia.cc koPartSelectDia_data.cc \
	koRuler.cc koTemplateChooseDia.cc koTabChooser.cc \
	koKoolBar.cc koUIUtils.cc

include_HEADERS = koAboutDia.h koPageLayoutDia.h \
	koPartSelectDia.h koPartSelectDia_data.h \
	koRuler.h koTemplateChooseDia.h koTabChooser.h \
	koKoolBar.h koUIUtils.h

METASOURCES = AUTO

####### Implicit rules



