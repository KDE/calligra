
INCLUDES = -I../lib/kofficecore -I../lib/kofficeui \
	-I../lib/koml $(all_includes) $(MICO_INCLUDES) -I$(idldir) -g
LDFLAGS	= $(all_libraries) $(MICO_LDFLAGS) $(KDE_RPATH) -g
LDADD = ../lib/kofficeui/libkofficeui.la \
	../lib/kofficecore/libkofficecore.la ../lib/koml/libkoml.la \
	-lop -lkfile -lkfm -lkdeui -lkdecore $(LIBMICO) -lqt -lXext -lX11 \
	-lkom  -ljpeg $(LIBSOCKET) -ldl

####### Files

bin_PROGRAMS = kword.bin

kword_bin_SOURCES = char.cc kword_main.cc kword_doc.cc kword_view.cc kword.cc kword_shell.cc \
		fc.cc font.cc format.cc kword_page.cc parag.cc paraglayout.cc \
		formatcollection.cc paragdia.cc imagecollection.cc image.cc \
		jpeg.cc formats.cc frame.cc

noinst_HEADERS = char.h kword_main.h kword_doc.h kword_view.h kword.h kword_shell.h \
		fc.h font.h format.h kword_page.h parag.h paraglayout.h \
		formatcollection.h paragdia.h imagecollection.h image.h \
		jpeg.h formats.h frame.h

SRCMETA =	kword_main.moc kword_view.moc kword_doc.moc kword_shell.moc paragdia.moc kword_page.moc

DISTCLEANFILES = $(idl_DATA:.idl=.cc) $(idl_HEADERS) $(SRCMETA)

idl_DATA = kword.idl
idl_HEADERS =  kword.h

kdelnk_DATA = kword_part.kdelnk
kdelnkdir = $(kde_datadir)/koffice/partlnk

SUBDIRS = toolbar templates

####### Implicit rules

SUFFIXES = .h .moc

.h.moc:
	$(MOC) $< -o $@      

####### Build rules

install-exec-local:
	$(INSTALL_SCRIPT) kword $(bindir)/kword

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot  *.cc && mv messages.po ../po/kword.pot

kword.h: kword.idl
	rm -f kword.h
	$(IDL) --query-server-for-narrow -I$(idldir) kword.idl

kword.cc: kword.h
kword_view.h: kword.h
kword_shell.h: kword.h
kword_doc.cc: kword_doc.moc
kword_main.cc: kword_main.moc
kword_shell.cc: kword_shell.moc
kword_view.cc: kword_view.moc
kword_main.cc: kword_view.h kword.h
char.cc: char.h kword.h
fc.cc: fc.h
font.cc: font.h
format.cc: format.h
kword_page.cc: kword_page.h
parag.cc: parag.h
paraglayout.cc: paraglayout.h kword.h kword_view.h
formatcollection.cc: formatcollection.h
paragdia.cc: paragdia.moc
kword_page.cc: kword_page.moc
imagecollection.cc: imagecollection.h
image.cc: image.h
kword_view.h: kword.h
frame.cc: frame.h


