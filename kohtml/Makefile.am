
INCLUDES = -I../lib/store -I../lib/kofficecore -I../lib/kofficeui \
	-I../lib/koml $(all_includes) $(MICO_INCLUDES) -I$(idldir)
LDFLAGS	= $(all_libraries) $(MICO_LDFLAGS) $(KDE_RPATH)
LDADD = ../lib/kofficeui/libkofficeui.la \
	../lib/kofficecore/libkofficecore.la ../lib/koml/libkoml.la \
	../lib/store/libkstore.la -lkspell -lkparts -lkpartsui -lkom \
	$(LIB_KHTML) $(LIB_KFILE) $(LIB_KIMGIO) -lkdeutil $(LIBMICO) 

####### Files

bin_PROGRAMS = kohtml.bin

kohtml_bin_SOURCES = kohtml_main.cc kohtml_doc.cc kohtml_view.cc \
	kohtml_shell.cc kohtml.cc khtmlwidget_patched.cc khtmlview_patched.cc\
	kfileio.cc

noinst_HEADERS = kohtml_main.h kohtml_doc.h kohtml_view.h kohtml_shell.h \
	kohtml.h khtmlwidget_patched.h khtmlview_patched.h kfileio.h

SRCMETA = kohtml_main.moc kohtml_view.moc kohtml_shell.moc kohtml_doc.moc \
	khtmlwidget_patched.moc khtmlview_patched.moc	

DISTCLEANFILES = $(idl_DATA:.idl=.cc) $(idl_HEADERS) $(SRCMETA)

idl_DATA = kohtml.idl
idl_HEADERS =  kohtml.h

kdelnk_DATA = kohtml_part.kdelnk
kdelnkdir = $(kde_datadir)/koffice/partlnk

####### Implicit rules

SUFFIXES = .h .moc

.h.moc:
	$(MOC) $< -o $@      

####### Build rules

install-exec-local:
	$(INSTALL_SCRIPT) kohtml $(bindir)/kohtml

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot  *.cc && mv messages.po ../po/kohtml.pot

kohtml.h: kohtml.idl
	rm -f kohtml.h
	$(IDL) --query-server-for-narrow --relative-paths -I../lib/kofficecore -I../lib/store -I$(idldir) kohtml.idl

kkohtml.cc: kohtml.h
kohtml_view.cc: kohtml.h kohtml_view.moc
kohtml_shell.cc: kohtml.h kohtml_shell.moc
kohtml_doc.cc: kohtml_doc.moc kohtml.h kfileio.h kohtml_doc.moc
kohtml_main.cc: kohtml_main.moc kohtml.h kohtml_main.moc
kfileio.cc: kfileio.h
khtmlview_patched.cc: khtmlview_patched.h khtmlwidget_patched.h khtmlview_patched.moc
khtmlwidget_patched.cc: khtmlwidget_patched.h khtmlwidget_patched.moc
