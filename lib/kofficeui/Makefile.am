####### General stuff

INCLUDES=	-I../kofficecore $(all_includes) $(MICO_INCLUDES) -I$(idldir)
LDFLAGS	=	$(all_libraries) -version-info 1:0:0
LIBADD  =           $(LIB_KDEUI)

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

SRCMETA = koAboutDia.moc koPageLayoutDia.moc koPartSelectDia.moc \
	koPartSelectDia_data.moc koPrintDia.moc \
	koRuler.moc koTemplateChooseDia.moc koTabChooser.moc \
	koKoolBar.moc

SUFFIXES = .h .moc

.h.moc:
	$(MOC) $< -o $@      

koAboutDia.cc: koAboutDia.moc
koPageLayoutDia.cc: koPageLayoutDia.moc
koPartSelectDia.cc: koPartSelectDia.moc
koPartSelectDia_data.cc: koPartSelectDia_data.moc
koPrintDia.cc: koPrintDia.moc
koRuler.cc: koRuler.moc
koTemplateChooseDia.cc: koTemplateChooseDia.moc
koTabChooser.cc: koTabChooser.moc
koKoolBar.cc: koKoolBar.moc
