
KDE_CXXFLAGS = $(USE_RTTI) -g
INCLUDES = $(KOFFICE_INCLUDES) $(all_includes)

####### Files

bin_PROGRAMS = kword
lib_LTLIBRARIES = libkword.la

kword_LDFLAGS = $(all_libraries) $(KDE_RPATH)
kword_LDADD = libkword.la

libkword_la_LDFLAGS = $(all_libraries) -avoid-version -module
libkword_la_LIBADD = ../lib/kofficeui/libkofficeui.la -lkspell -lkformula

libkword_la_SOURCES = kword_doc.cc searchdia.cc char.cc kword_view.cc \
                kword_shell.cc fc.cc font.cc format.cc kword_page.cc parag.cc \
                paraglayout.cc formatcollection.cc paragdia.cc \
                imagecollection.cc image.cc kword_frame.cc framedia.cc \
                stylist.cc kcharselectdia.cc kword_utils.cc preview.cc \
                tabledia.cc insdia.cc deldia.cc variable.cc docstruct.cc \
                kword_undo.cc clipbrd_dnd.cc footnote.cc footnotedia.cc \
		autoformat.cc autoformatdia.cc variabledlgs.cc \
                serialletter.cc contents.cc kword_factory.cc

kword_SOURCES = main.cc

noinst_HEADERS = char.h kword_doc.h kword_view.h kword_shell.h \
		fc.h font.h format.h kword_page.h parag.h paraglayout.h \
		formatcollection.h paragdia.h imagecollection.h image.h \
		kword_frame.h framedia.h stylist.h  kcharselectdia.h \
		kword_utils.h searchdia.h preview.h tabledia.h insdia.h \
                deldia.h variable.h docstruct.h kword_undo.h clipbrd_dnd.h \
                footnote.h footnotedia.h autoformat.h autoformatdia.h \
                variabledlgs.h serialletter.h contents.h kword_factory.h

METASOURCES =	kword_view.moc kword_doc.moc kword_shell.moc paragdia.moc \
		kword_page.moc framedia.moc stylist.moc kcharselectdia.moc \
		searchdia.moc tabledia.moc insdia.moc deldia.moc \
		docstruct.moc kword_undo.moc clipbrd_dnd.moc footnotedia.moc \
		autoformatdia.moc variabledlgs.moc serialletter.moc \
		kword_factory.moc preview.moc

kdelnk_DATA = kword.desktop
kdelnkdir = $(kde_appsdir)/Applications/KOffice

kdemime_DATA = x-kword.desktop
kdemimedir = $(kde_mimedir)/application

rcdir = $(kde_datadir)/kword
rc_DATA = kword.rc kword_shell.rc

SUBDIRS = . toolbar templates dtd pics

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot  *.cc && mv messages.po $(podir)/kword.pot
