
KDE_CXXFLAGS = $(USE_RTTI) 
INCLUDES = $(KOFFICE_INCLUDES) $(all_includes)
LDFLAGS	= $(all_libraries) $(KDE_RPATH)
LDADD = ../lib/kofficeui/libkofficeui.la -lkspell -lkformula $(LIB_KIMGIO)

####### Files

bin_PROGRAMS = kword

kword_SOURCES = kword_doc.cc searchdia.cc char.cc kword_view.cc kword_shell.cc \
		fc.cc font.cc format.cc kword_page.cc parag.cc paraglayout.cc \
		formatcollection.cc paragdia.cc imagecollection.cc image.cc \
		kword_frame.cc framedia.cc stylist.cc kcharselectdia.cc \
		kword_utils.cc preview.cc tabledia.cc insdia.cc deldia.cc variable.cc \
		docstruct.cc kword_undo.cc clipbrd_dnd.cc footnote.cc footnotedia.cc \
		autoformat.cc autoformatdia.cc variabledlgs.cc serialletter.cc \
		contents.cc kword_factory.cc main.cc

noinst_HEADERS = char.h kword_doc.h kword_view.h kword_shell.h \
		fc.h font.h format.h kword_page.h parag.h paraglayout.h \
		formatcollection.h paragdia.h imagecollection.h image.h \
		kword_frame.h framedia.h stylist.h  kcharselectdia.h \
		kword_utils.h searchdia.h preview.h tabledia.h insdia.h deldia.h variable.h \
		docstruct.h kword_undo.h clipbrd_dnd.h footnote.h footnotedia.h \
		autoformat.h autoformatdia.h variabledlgs.h serialletter.h \
		contents.h kword_factory.h

METASOURCES =	kword_view.moc kword_doc.moc kword_shell.moc paragdia.moc \
		kword_page.moc framedia.moc stylist.moc kcharselectdia.moc \
		searchdia.moc tabledia.moc insdia.moc deldia.moc docstruct.moc kword_undo.moc \
		clipbrd_dnd.moc footnotedia.moc autoformatdia.moc variabledlgs.moc serialletter.moc \
		kword_factory.moc

kdelnk_DATA = kword.desktop
kdelnkdir = $(kde_appsdir)/KOffice

kdemime_DATA = x-kword.desktop
kdemimedir = $(kde_mimedir)/application

rcdir = $(kde_datadir)/kword
rc_DATA = kword.rc kword_shell.rc

######### filters !!!!!!!!!
SUBDIRS = . toolbar templates dtd pics

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot  *.cc && mv messages.po ../po/kword.pot

