
INCLUDES = $(KOFFICE_INCLUDES) $(all_includes) $(MICO_INCLUDES) -I$(idldir)
LDFLAGS	= -g $(all_libraries) $(MICO_LDFLAGS) $(KDE_RPATH)
LDADD = ../lib/kofficeui/libkofficeui.la -lkspell $(LIBQIMGIO) $(LIB_KIMGIO)

####### Files

bin_PROGRAMS = kword

kword_SOURCES = kword_doc.cc char.cc kword_main.cc kword_view.cc kword.cc kword_shell.cc \
		fc.cc font.cc format.cc kword_page.cc parag.cc paraglayout.cc \
		formatcollection.cc paragdia.cc imagecollection.cc image.cc \
		formats.cc frame.cc framedia.cc stylist.cc kcharselectdia.cc \
		kword_utils.cc searchdia.cc preview.cc tabledia.cc insdia.cc deldia.cc variable.cc \
		docstruct.cc kword_undo.cc clipbrd_dnd.cc footnote.cc footnotedia.cc \
		autoformat.cc autoformatdia.cc

noinst_HEADERS = char.h kword_main.h kword_doc.h kword_view.h kword.h kword_shell.h \
		fc.h font.h format.h kword_page.h parag.h paraglayout.h \
		formatcollection.h paragdia.h imagecollection.h image.h \
		formats.h frame.h framedia.h stylist.h  kcharselectdia.h \
		kword_utils.h searchdia.h preview.h tabledia.h insdia.h deldia.h variable.h \
		docstruct.h kword_undo.h clipbrd_dnd.h footnote.h footnotedia.h \
		autoformat.h autoformatdia.h

METASOURCES =	kword_main.moc kword_view.moc kword_doc.moc kword_shell.moc paragdia.moc \
		kword_page.moc framedia.moc stylist.moc kcharselectdia.moc \
		searchdia.moc tabledia.moc insdia.moc deldia.moc docstruct.moc kword_undo.moc \
		clipbrd_dnd.moc footnotedia.moc autoformatdia.moc

DISTCLEANFILES = $(idl_DATA:.idl=.cc) $(idl_HEADERS)

idl_DATA = kword.idl
idl_HEADERS =  kword.h

kdelnk_DATA = kword.desktop
kdelnkdir = $(kde_appsdir)/KOffice

kdemime_DATA = x-kword.kdelnk
kdemimedir = $(kde_mimedir)/application

SUBDIRS = . toolbar templates filters dtd

####### Implicit rules

####### Build rules

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot  *.cc && mv messages.po ../po/kword.pot

kword.cc: kword.h
kword.h: kword.idl $(micodir)/include/CORBA.h
	rm -f kword.h
	$(IDL) --query-server-for-narrow --relative-paths -I$(top_srcdir)/lib/kofficecore -I$(top_srcdir)/lib/store -I$(idldir) $(srcdir)/kword.idl

fc.cc: fc.h
font.cc: font.h
format.cc: format.h
kword_page.cc: kword_page.h
parag.cc: parag.h
paraglayout.cc: paraglayout.h kword_view.h
formatcollection.cc: formatcollection.h
imagecollection.cc: imagecollection.h
image.cc: image.h
kword_view.h: kword.h
frame.cc: frame.h
kword_utils.cc: kword_utils.h
stylist.cc: kword.h
insdia.cc: insdia.h
tabledia.cc: tabledia.h
deldia.cc: deldia.h
variable.cc: variable.h
docstruct.cc: docstruct.h
kword_undo.cc: kword_undo.h
clipbrd_dnd.cc: clipbrd_dnd.h
footnotedia.cc: footnotedia.h
autoformatdia.cc: autoformatdia.h
