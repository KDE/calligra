
INCLUDES  = $(MICO_INCLUDES)  -I../lib/koml \
	-I../lib/kofficecore -I../lib/kofficeui $(all_includes) -I$(idldir)
LDFLAGS =   $(all_libraries) $(MICO_LDFLAGS) $(KDE_RPATH)
LDADD =  ../lib/kofficeui/libkofficeui.la \
	../lib/kofficecore/libkofficecore.la \
	../lib/koml/libkoml.la -lkfile -lkfm -lkom -lop -lkdeui -lkdecore \
	-lqt $(LIBMICO) -lXext -lX11 $(LIBSOCKET) 

####### Files

bin_PROGRAMS = kimage.bin

kimage_bin_SOURCES = img_main.cc img_doc.cc img_view.cc img.cc img_shell.cc
noinst_HEADERS = img_main.h img_doc.h img_view.h img.h img_shell.h
SRCMETA =	img_main.moc img_view.moc img_doc.moc img_shell.moc

DISTCLEANFILES = $(idl_DATA:.idl=.cc) $(idl_HEADERS) $(SRCMETA)

idl_DATA = img.idl
idl_HEADERS =  img.h

kdelnk_DATA = kimage_part.kdelnk
kdelnkdir = $(kde_datadir)/koffice/partlnk

####### Implicit rules

SUFFIXES = .h .moc

.h.moc:
	$(MOC) $< -o $@      

####### Build rules

bin_SCRIPTS = kimage

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot -x $(includedir)/corba.pot  *.cc && mv messages.po ../po/kimage.pot

img.h: img.idl
	$(IDL) --relative-paths --query-server-for-narrow -I$(idldir) img.idl

img.cc: img.h
img_view.h: img.h
img_shell.h: img.h
img_doc.cc: img_doc.moc
img_main.cc: img_main.moc
img_shell.cc: img_shell.moc
img_view.cc: img_view.moc
img_main.cc: img_view.h img.h



