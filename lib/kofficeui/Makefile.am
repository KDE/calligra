####### General stuff

INCLUDES=	-I$(top_srcdir)/lib/kofficecore -I$(top_builddir)/lib/kofficecore $(all_includes) $(MICO_INCLUDES) -I$(idldir)
LDFLAGS	=	$(all_libraries) -version-info 1:0:0
LIBADD  =       $(LIB_KDEUI)

####### Files

SUBDIRS = pics test

lib_LTLIBRARIES = libkofficeui.la

libkofficeui_la_SOURCES = koAboutDia.cc koPageLayoutDia.cc \
	koPartSelectDia.cc koPartSelectDia_data.cc koPrintDia.cc \
	koRuler.cc koTemplateChooseDia.cc koTabChooser.cc \
	koKoolBar.cc

include_HEADERS = koAboutDia.h koPageLayoutDia.h \
	koPartSelectDia.h koPartSelectDia_data.h koPrintDia.h \
	koRuler.h koTemplateChooseDia.h koTabChooser.h \
	koKoolBar.h

METASOURCES = koAboutDia.moc koPartSelectDia.moc koRuler.moc \
	  koKoolBar.moc koPartSelectDia_data.moc koTabChooser.moc \
	  koPageLayoutDia.moc koPrintDia.moc koTemplateChooseDia.moc

####### Implicit rules



