# $Id$

SUBDIRS = . pics

INCLUDES = $(KOFFICE_INCLUDES) $(all_includes) $(MICO_INCLUDES) -I$(idldir)
LDFLAGS	= $(all_libraries) $(KDE_RPATH)
LDADD = ../lib/kofficeui/libkofficeui.la 

####### Files

bin_PROGRAMS = kformula

kformula_SOURCES =  kformula_shell.cc  kformula_view.cc matrixwidget.cc \
	kformula_main.cc kformula_doc.cc kformula.idl BasicElement.cc \
	RootElement.cc TextElement.cc BracketElement.cc FractionElement.cc\
	MatrixElement.cc PrefixedElement.cc formula_container.cc

noinst_HEADERS = kformula_main.h kformula_doc.h kformula_view.h formuladef.h \
	kformula_shell.h BasicElement.h RootElement.h TextElement.h\
	BracketElement.h FractionElement.h MatrixElement.h PrefixedElement.h \
	matrixwidget.h formula_container.h

METASOURCES = kformula_main.moc kformula_view.moc kformula_doc.moc \
	kformula_shell.moc matrixwidget.moc formula_container.moc

idl_DATA = kformula.idl
idl_HEADERS = kformula.h

kde_icon_DATA = kformula.xpm

kdelnk_DATA = kformula.desktop
kdelnkdir = $(kde_appsdir)/KOffice

####### Implicit rules

####### Build rules

IDL_FLAGS = --query-server-for-narrow --relative-paths -I$(top_srcdir)/lib/kofficecore -I$(top_srcdir)/lib/store -I$(idldir)

messages:
	$(XGETTEXT) -C -ki18n -kklocale -x $(includedir)/kde.pot *.cc && mv messages.po ../po/kformula.pot

