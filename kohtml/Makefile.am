
INCLUDES = $(KOFFICE_INCLUDES) $(all_includes) $(MICO_INCLUDES) -I$(idldir)
LDFLAGS	= $(all_libraries) $(MICO_LDFLAGS) $(KDE_RPATH)
LDADD = $(KOFFICE_LIBS) -lkparts -lkpartsui -lkom $(LIB_KIO)  \
	$(LIB_KHTML) $(LIB_KFILE) $(LIBMICO)

####### Files

bin_PROGRAMS = kohtml.bin

kohtml_bin_SOURCES = kohtml_main.cc kohtml_doc.cc kohtml_view.cc \
	kohtml_shell.cc kohtml.cc khtmlwidget_patched.cc khtmlview_patched.cc \
	kfileio.cc kpropcheck.cc kpropcolor.cc kpropcombo.cc kpropdlg.cc \
	kpropfont.cc kpropgroup.cc kpropradio.cc kpropspin.cc kproptext.cc \
	settingsdlg.cc edithtmldlg.cc kohtmljob.cc

noinst_HEADERS = kohtml_main.h kohtml_doc.h kohtml_view.h kohtml_shell.h \
	kohtml.h khtmlwidget_patched.h khtmlview_patched.h kfileio.h \
	kpropcheck.h kpropcolor.h kpropcombo.h kpropdlg.h kpropfont.h \
	kpropgroup.h kpropradio.h kpropspin.h kproptext.h settingsdlg.h \
	edithtmldlg.h kohtmljob.h

SRCMETA = kohtml_main.moc kohtml_view.moc kohtml_shell.moc kohtml_doc.moc \
	khtmlwidget_patched.moc khtmlview_patched.moc kpropcheck.moc \
	kpropcolor.moc kpropcombo.moc kpropdlg.moc kpropfont.moc kpropgroup.moc \
	kpropradio.moc kpropspin.moc kproptext.moc settingsdlg.moc edithtmldlg.moc \
	kohtmljob.moc

DISTCLEANFILES = $(idl_DATA:.idl=.cc) $(idl_HEADERS) $(SRCMETA)

idl_DATA = kohtml.idl
idl_HEADERS =  kohtml.h

kdelnk_DATA = kohtml_part.kdelnk
kdelnkdir = $(kde_datadir)/koffice/partlnk

pics_DATA = kohtml.xpm
picsdir = $(kde_icondir)/kohtml.xpm

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
	$(IDL) --query-server-for-narrow --relative-paths -I$(top_srcdir)/lib/kofficecore -I$(top_srcdir)/lib/store -I$(idldir) $(srcdir)/kohtml.idl

kkohtml.cc: kohtml.h
kohtml_view.cc: kohtml.h kohtml_view.moc settingsdlg.h
kohtml_shell.cc: kohtml.h kohtml_shell.moc
kohtml_doc.cc: kohtml_doc.moc kohtml.h kfileio.h kohtml_doc.moc
kohtml_main.cc: kohtml_main.moc kohtml.h kohtml_main.moc
kfileio.cc: kfileio.h
khtmlview_patched.cc: khtmlview_patched.h khtmlwidget_patched.h khtmlview_patched.moc
khtmlwidget_patched.cc: khtmlwidget_patched.h khtmlwidget_patched.moc
kpropcheck.cc: kpropcheck.h kpropcheck.moc
kpropcolor.cc: kpropcolor.h kpropcolor.moc
kpropcombo.cc: kpropcombo.h kpropcombo.moc
kpropdlg.cc: kpropdlg.h kpropdlg.moc
kpropfont.cc: kpropfont.h kpropfont.moc
kpropgroup.cc: kpropgroup.h kpropgroup.moc
kpropradio.cc: kpropradio.h kpropradio.moc
kpropspin.cc: kpropspin.h kpropspin.moc
kproptext.cc: kproptext.h kproptext.moc
settingsdlg.cc: settingsdlg.h settingsdlg.moc
edithtmldlg.cc: edithtmldlg.h edithtmldlg.moc
kohtmljob.cc: kohtmljob.h kohtmljob.moc