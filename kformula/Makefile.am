# $Id$

SUBDIRS = pics

INCLUDES = $(KOFFICE_INCLUDES) $(all_includes) $(MICO_INCLUDES) -I$(idldir)
LDFLAGS	= $(all_libraries) $(MICO_LDFLAGS) $(KDE_RPATH)
LDADD = $(KOFFICE_LIBS) -lkparts -lkpartsui -lkom $(LIB_KIO) $(LIB_KFILE) \
	 $(LIB_KIMGIO) $(LIBMICO) 

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
METASOURCES = kformula_main.moc kformula_view.moc kformula_doc.moc \
	kformula_shell.moc matrixwidget.moc

DISTCLEANFILES = $(idl_DATA:.idl=.cc) $(idl_HEADERS) $(METASOURCES)

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

####### Build rules

install-exec-local:
	$(INSTALL_SCRIPT) $(srcdir)/kformula $(bindir)/kformula

kformula.h: kformula.idl
	rm -f kformula.h
	$(IDL) --query-server-for-narrow --relative-paths -I$(top_srcdir)/lib/kofficecore -I$(top_srcdir)/lib/store -I$(idldir) $(srcdir)/kformula.idl

messages:
	$(XGETTEXT) -C -ki18n -kklocale -x $(includedir)/kde.pot *.cc && mv messages.po ../po/kformula.pot

kformula.cc: kformula.h
kformula_view.h: kformula.h
kformula_shell.h: kformula.h
$(srcdir)/kformula_main.cc: kformula_view.h kformula.h
$(srcdir)/kformula_view.cc: kformula.h
