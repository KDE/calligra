
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

METASOURCES = kohtml_main.moc kohtml_view.moc kohtml_shell.moc kohtml_doc.moc \
	khtmlwidget_patched.moc khtmlview_patched.moc kpropcheck.moc \
	kpropcolor.moc kpropcombo.moc kpropdlg.moc kpropfont.moc kpropgroup.moc \
	kpropradio.moc kpropspin.moc kproptext.moc settingsdlg.moc edithtmldlg.moc \
	kohtmljob.moc

DISTCLEANFILES = $(idl_DATA:.idl=.cc) $(idl_HEADERS) $(METASOURCES)

idl_DATA = kohtml.idl
idl_HEADERS =  kohtml.h

kdelnk_DATA = kohtml_part.kdelnk
kdelnkdir = $(kde_datadir)/koffice/partlnk

pics_DATA = kohtml.xpm
picsdir = $(kde_icondir)/kohtml.xpm

####### Implicit rules

####### Build rules

install-exec-local:
	$(INSTALL_SCRIPT) $(srcdir)/kohtml $(bindir)/kohtml

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot  *.cc && mv messages.po ../po/kohtml.pot

kohtml.h: kohtml.idl
	rm -f kohtml.h
	$(IDL) --query-server-for-narrow --relative-paths -I$(top_srcdir)/lib/kofficecore -I$(top_srcdir)/lib/store -I$(idldir) $(srcdir)/kohtml.idl

kkohtml.cc: kohtml.h
kohtml_view.cc: kohtml.h settingsdlg.h
kohtml_shell.cc: kohtml.h 
kohtml_doc.cc: kohtml.h kfileio.h
kohtml_main.cc:  kohtml.h
kfileio.cc: kfileio.h
khtmlview_patched.cc: khtmlview_patched.h khtmlwidget_patched.h 
khtmlwidget_patched.cc: khtmlwidget_patched.h 
kpropcheck.cc: kpropcheck.h
kpropcolor.cc: kpropcolor.h
kpropcombo.cc: kpropcombo.h
kpropdlg.cc: kpropdlg.h
kpropfont.cc: kpropfont.h 
kpropgroup.cc: kpropgroup.h 
kpropradio.cc: kpropradio.h 
kpropspin.cc: kpropspin.h 
kproptext.cc: kproptext.h 
settingsdlg.cc: settingsdlg.h 
edithtmldlg.cc: edithtmldlg.h 
kohtmljob.cc: kohtmljob.h 
