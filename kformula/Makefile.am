# $Id$

SUBDIRS = pics

INCLUDES = -I../lib/store -I../lib/kofficecore -I../lib/kofficeui \
	-I../lib/koml $(all_includes) $(MICO_INCLUDES) -I$(idldir)
LDFLAGS	= $(all_libraries) $(MICO_LDFLAGS) $(KDE_RPATH)
LDADD = ../lib/kofficeui/libkofficeui.la \
	../lib/kofficecore/libkofficecore.la ../lib/koml/libkoml.la \
	../lib/store/libkstore.la \
	-lkparts -lkpartsui -lkom -lkfile -lkfm $(LIB_KIMGIO) -lkdeui \
	-lkdeutil -lkdecore $(LIBMICO) -lXext $(LIB_QT $(LIBDL)

####### Files

bin_PROGRAMS = kformula.bin

kformula_bin_SOURCES =  kformula_view.cc matrixwidget.cc kformula_main.cc \
	kformula_doc.cc \
	kformula.cc kformula_shell.cc BasicElement.cc RootElement.cc \
	TextElement.cc BracketElement.cc FractionElement.cc\
	MatrixElement.cc PrefixedElement.cc 
noinst_HEADERS = kformula_main.h kformula_doc.h kformula_view.h formuladef.h \
	kformula.h kformula_shell.h BasicElement.h RootElement.h TextElement.h\
	BracketElement.h FractionElement.h MatrixElement.h PrefixedElement.h \
	matrixwidget.h
SRCMETA = kformula_main.moc kformula_view.moc kformula_doc.moc \
	kformula_shell.moc matrixwidget.moc

DISTCLEANFILES = $(idl_DATA:.idl=.cc) $(idl_HEADERS) $(SRCMETA)

IDL_INCLUDES = -I$(idldir)

idl_DATA = kformula.idl
idl_HEADERS = kformula.h
pics_DATA = kformula.xpm
picsdir = $(kde_icondir)
kdelnk_DATA = kformula_part.kdelnk
kdelnkdir = $(kde_datadir)/koffice/partlnk

uninstall-local:
	-rm $(bindir)/kformula

####### Implicit rules

SUFFIXES = .h .moc

.h.moc:
	$(MOC) $< -o $@      

####### Build rules

install-exec-local:
	$(INSTALL_SCRIPT) kformula $(bindir)/kformula

kformula.h: kformula.idl
	rm -f kformula.h
	$(IDL) --query-server-for-narrow --relative-paths -I../lib/kofficecore -I../lib/store -I$(idldir) kformula.idl

messages:
	$(XGETTEXT) -C -ki18n -kklocale -x $(includedir)/kde.pot *.cc && mv messages.po ../po/kformula.pot

kformula.cc: kformula.h
kformula_view.h: kformula.h
kformula_shell.h: kformula.h
kformula_doc.cc: kformula_doc.moc
kformula_main.cc: kformula_main.moc kformula.h
kformula_shell.cc: kformula_shell.moc
kformula_view.cc: kformula_view.moc
kformula_main.cc: kformula_view.h kformula.h
matrixwidget.cc: matrixwidget.moc
