
INCLUDES = $(KOFFICE_INCLUDES) $(all_includes) $(MICO_INCLUDES) -I$(idldir)
LDFLAGS	= $(all_libraries) $(MICO_LDFLAGS) $(KDE_RPATH)
LDADD = $(KOFFICE_LIBS) -lkspell -lkparts -lkpartsui -lkom $(LIB_KIO) \
	$(LIB_KFILE) $(LIB_KIMGIO)  $(LIBMICO) 

####### Files

bin_PROGRAMS = kword.bin

kword_bin_SOURCES = char.cc kword_main.cc kword_doc.cc kword_view.cc kword.cc kword_shell.cc \
		fc.cc font.cc format.cc kword_page.cc parag.cc paraglayout.cc \
		formatcollection.cc paragdia.cc imagecollection.cc image.cc \
		formats.cc frame.cc framedia.cc stylist.cc kcharselect.cc kcharselectdia.cc \
		kword_utils.cc searchdia.cc preview.cc tabledia.cc

noinst_HEADERS = char.h kword_main.h kword_doc.h kword_view.h kword.h kword_shell.h \
		fc.h font.h format.h kword_page.h parag.h paraglayout.h \
		formatcollection.h paragdia.h imagecollection.h image.h \
		formats.h frame.h framedia.h stylist.h  kcharselect.h kcharselectdia.h \
		kword_utils.h searchdia.h preview.h tabledia.h

SRCMETA =	kword_main.moc kword_view.moc kword_doc.moc kword_shell.moc paragdia.moc \
		kword_page.moc framedia.moc stylist.moc kcharselect.moc kcharselectdia.moc \
		searchdia.moc tabledia.moc

DISTCLEANFILES = $(idl_DATA:.idl=.cc) $(idl_HEADERS) $(SRCMETA)

idl_DATA = kword.idl
idl_HEADERS =  kword.h

kdelnk_DATA = kword_part.kdelnk
kdelnkdir = $(kde_datadir)/koffice/partlnk

SUBDIRS = . toolbar templates

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
	$(IDL) --query-server-for-narrow --relative-paths -I$(top_srcdir)/lib/kofficecore -I$(top_srcdir)/lib/store -I$(idldir) $(srcdir)/kword.idl

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
framedia.cc: framedia.moc
stylist.cc: stylist.moc
kcharselect.cc: kcharselect.moc
kcharselectdia.cc: kcharselectdia.moc
kword_utils.cc: kword_utils.h
stylist.cc: kword.h
searchdia.cc: searchdia.moc
tabledia.cc: tabledia.moc