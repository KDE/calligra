# $Id$

SUBDIRS = . pics

INCLUDES = $(KOFFICE_INCLUDES) $(all_includes) $(MICO_INCLUDES) -I$(idldir)
LDFLAGS	= $(all_libraries) $(KDE_RPATH)
LDADD = ../lib/kofficeui/libkofficeui.la 

####### Files

bin_PROGRAMS = kformula.bin

kformula_bin_SOURCES =  kformula_shell.cc  kformula_view.cc matrixwidget.cc \
	kformula_main.cc kformula_doc.cc kformula.cc BasicElement.cc \
	RootElement.cc TextElement.cc BracketElement.cc FractionElement.cc\
	MatrixElement.cc PrefixedElement.cc formula_container.cc

noinst_HEADERS = kformula_main.h kformula_doc.h kformula_view.h formuladef.h \
	kformula.h kformula_shell.h BasicElement.h RootElement.h TextElement.h\
	BracketElement.h FractionElement.h MatrixElement.h PrefixedElement.h \
	matrixwidget.h formula_container.h

METASOURCES = kformula_main.moc kformula_view.moc kformula_doc.moc \
	kformula_shell.moc matrixwidget.moc formula_container.moc

DISTCLEANFILES = $(idl_DATA:.idl=.cc) $(idl_HEADERS) $(METASOURCES)

IDL_INCLUDES = -I$(idldir)

idl_DATA = kformula.idl
idl_HEADERS = kformula.h
pics_DATA = kformula.xpm
picsdir = $(kde_icondir)
kdelnk_DATA = kformula.desktop
kdelnkdir = $(kde_appsdir)/KOffice

uninstall-local:
	-rm $(bindir)/kformula

####### Implicit rules

####### Build rules

install-exec-local:
	$(INSTALL_SCRIPT) $(srcdir)/kformula $(bindir)/kformula

kformula.h: kformula.idl $(micodir)/include/CORBA.h
	rm -f kformula.h
	$(IDL) --query-server-for-narrow --relative-paths -I$(top_srcdir)/lib/kofficecore -I$(top_srcdir)/lib/store -I$(idldir) $(srcdir)/kformula.idl

messages:
	$(XGETTEXT) -C -ki18n -kklocale -x $(includedir)/kde.pot *.cc && mv messages.po ../po/kformula.pot

kformula.cc: kformula.h
kformula_view.h: kformula.h
kformula_shell.h: kformula.h
$(srcdir)/kformula_main.cc: kformula_view.h kformula.h
$(srcdir)/kformula_view.cc: kformula.h
