####### General stuff

INCLUDES=	$(all_includes) $(MICO_INCLUDES) -I../kofficecore -I$(idldir)
LDFLAGS	=	$(all_libraries) -version-info 1:0:0
LIBADD  =       -lkdeui -lkdecore -lqt -lXext -lX11 -ldl

####### Files

SUBDIRS = pics

lib_LTLIBRARIES = libkofficeui.la

libkofficeui_la_SOURCES = koAboutDia.cc koPageLayoutDia.cc \
	koPartSelectDia.cc koPartSelectDia_data.cc koPrintDia.cc

include_HEADERS = koAboutDia.h koPageLayoutDia.h \
	koPartSelectDia.h koPartSelectDia_data.h koPrintDia.h

SRCMETA = koAboutDia.moc koPageLayoutDia.moc koPartSelectDia.moc \
	koPartSelectDia_data.moc koPrintDia.moc

SUFFIXES = .h .moc

.h.moc:
	$(MOC) $< -o $@      

koAboutDia.cc: koAboutDia.moc
koPageLayoutDia.cc: koPageLayoutDia.moc
koPartSelectDia.cc: koPartSelectDia.moc
koPartSelectDia_data.cc: koPartSelectDia_data.moc
koPrintDia.cc: koPrintDia.moc
