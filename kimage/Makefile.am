
INCLUDES  = $(PYTHONINC) $(MICO_INCLUDES) -I../lib/store -I../lib/koml -I../lib/kofficecore -I../lib/kofficeui -I../lib/interface $(all_includes) -I$(idldir)
LDFLAGS   = $(LIB_KIMGIO) $(all_libraries) $(PYTHONLIB) $(MICO_LDFLAGS) $(KDE_RPATH)
LDADD = ../lib/kofficeui/libkofficeui.la \
	../lib/kofficecore/libkofficecore.la ../lib/koml/libkoml.la \
	../lib/store/libkstore.la -lkparts -lkpartsui \
	-lkom -lkio -lkdeutil $(LIBMICO) $(LIBPYTHON) $(LIB_KFILE) 

####### Files

bin_PROGRAMS = kimage.bin

kimage_bin_SOURCES = kimage_main.cc kimage_shell.cc kimage_doc.cc \
		     kimage.cc kimage_view.cc

noinst_HEADERS = kimage_main.h kimage_shell.h kimage_doc.h \
	         kimage.h kimage_view.h

SRCMETA = kimage_main.moc kimage_shell.moc kimage_doc.moc \
	  kimage_view.moc

idl_DATA = kimage.idl
idl_HEADERS = kimage.h

DISTCLEANFILES = $(idl_DATA:.idl=.cc) $(idl_HEADERS) $(SRCMETA)

kdelnk_DATA = kimage_part.kdelnk
kdelnkdir = $(kde_datadir)/koffice/partlnk

####### Implicit rules

SUFFIXES = .h .moc

.h.moc: 
	$(MOC) $< -o $@

####### Build rules

IDL_FLAGS = --query-server-for-narrow --relative-paths -I../lib/kofficecore -I../lib/store -I../lib/interfaces -I$(idldir)

install-exec-local:
	$(INSTALL_SCRIPT) kimage $(bindir)/kimage

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot -x $(includedir)/corba.pot  *.cc && mv messages.po ../po/kimage.pot

kimage.h: kimage.idl
	$(IDL) $(IDL_FLAGS) kimage.idl

kimage.cc: kimage.h
kimage_doc.cc: kimage.h kimage_doc.moc
kimage_main.cc: kimage.h kimage_main.moc
kimage_shell.cc: kimage.h kimage_shell.moc
kimage_view.cc: kimage.h kimage_view.moc
