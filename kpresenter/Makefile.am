
INCLUDES = $(MICO_INCLUDES) -I../lib/kofficecore -I../lib/kofficeui \
	-I../lib/koml $(all_includes) -I$(idldir)
LDFLAGS	= $(all_libraries) $(MICO_LDFLAGS) $(KDE_RPATH)
LDADD = autoformEdit/atfinterpreter.o autoformEdit/afchoose.o \
	../lib/kofficeui/libkofficeui.la \
	../lib/kofficecore/libkofficecore.la ../lib/koml/libkoml.la \
	-lop -lkfile -lkfm -lkdeui -lkdecore $(LIBMICO) -lqt -lXext -lX11 \
	-lkom  -ljpeg $(LIBSOCKET) -ldl

####### Files

bin_PROGRAMS = kpresenter.bin

kpresenter_bin_SOURCES = kpresenter_main.cc kpresenter_doc.cc kpresenter_view.cc \
	kpresenter_shell.cc kpresenter.cc graphobj.cc \
	backdia.cc page.cc ktextobject.cc styledia.cc \
	jpeg.cc formats.cc qwmf.cc kenumlistdia.cc kcharselect.cc \
	kcharselectdia.cc optiondia.cc pgconfdia.cc effectdia.cc

noinst_HEADERS = kpresenter_main.h kpresenter_doc.h kpresenter_view.h \
		kpresenter_shell.h kpresenter.h graphobj.h \
		backdia.h page.h ktextobject.h styledia.h \
		jpeg.h formats.h qwmf.h global.h \
		kenumlistdia.h kcharselect.h kcharselectdia.h optiondia.h \
		pgconfdia.h effectdia.h

SRCMETA =	kpresenter_main.moc kpresenter_doc.moc kpresenter_view.moc \
		kpresenter_shell.moc graphobj.moc moc \
		page.moc ktextobject.mocbackdia.moc styledia.moc \
		kenumlistdia.moc kcharselect.moc kcharselectdia.moc optiondia.moc \
		pgconfdia.moc effectdia.moc

idl_DATA =	kpresenter.idl
idl_HEADER =    kpresenter.h

kdelnk_DATA = kpresenter.kdelnk
kdelnkdir = $(kde_datadir)/koffice/partlnk

SUBDIRS = autoformEdit autoforms pics toolbar templates

####### Implicit rules

SUFFIXES = .h .moc 

.h.moc:
	$(MOC) $< -o $@      

####### Build rules

install-exec-local:
	$(INSTALL_SCRIPT) kpresenter $(bindir)/kpresenter

kpresenter.h: kpresenter.idl
	-rm -f kpresenter.h
	$(IDL) --query-server-for-narrow -I$(idldir) kpresenter.idl

kpresenter.cc: kpresenter.h
kpresenter_doc.cc: kpresenter_doc.moc
kpresenter_main.cc: kpresenter.h kpresenter_main.moc
kpresenter_shell.cc: kpresenter.h kpresenter_shell.moc
kpresenter_view.cc: kpresenter.h kpresenter_view.moc
graphobj.cc: graphobj.moc
backdia.cc: backdia.moc
page.cc: page.moc
ktextobject.cc: ktextobject.moc
styledia.cc: styledia.moc
kenumlistdia.cc: kenumlistdia.moc
kcharselect.cc: kcharselect.moc
kcharselectdia.cc: kcharselectdia.moc
optiondia.cc: optiondia.moc
pgconfdia.cc: pgconfdia.moc
effectdia.cc: effectdia.moc


