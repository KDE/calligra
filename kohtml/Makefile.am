
INCLUDES = $(KOFFICE_INCLUDES) $(all_includes) $(MICO_INCLUDES) -I$(idldir)
LDFLAGS	= $(all_libraries) $(MICO_LDFLAGS) $(KDE_RPATH)
LDADD = ../lib/kofficeui/libkofficeui.la $(LIB_KHTML) $(LIB_KFM)

####### Files

bin_PROGRAMS = kohtml.bin

kohtml_bin_SOURCES = kohtml_main.cc kohtml_doc.cc kohtml_view.cc \
	kohtml_shell.cc kohtml.cc htmwidget.cc htmview.cc \
	kfileio.cc kpropcheck.cc kpropcolor.cc kpropcombo.cc kpropdlg.cc \
	kpropfont.cc kpropgroup.cc kpropradio.cc kpropspin.cc kproptext.cc \
	settingsdlg.cc edithtmldlg.cc kohtmljob.cc openurldlg.cc

noinst_HEADERS = kohtml_main.h kohtml_doc.h kohtml_view.h kohtml_shell.h \
	kohtml.h htmwidget.h htmview.h kfileio.h \
	kpropcheck.h kpropcolor.h kpropcombo.h kpropdlg.h kpropfont.h \
	kpropgroup.h kpropradio.h kpropspin.h kproptext.h settingsdlg.h \
	edithtmldlg.h kohtmljob.h openurldlg.h

METASOURCES = kohtml_main.moc kohtml_view.moc kohtml_shell.moc kohtml_doc.moc \
	htmwidget.moc htmview.moc kpropcheck.moc \
	kpropcolor.moc kpropcombo.moc kpropdlg.moc kpropfont.moc kpropgroup.moc \
	kpropradio.moc kpropspin.moc kproptext.moc settingsdlg.moc edithtmldlg.moc \
	kohtmljob.moc openurldlg.moc

DISTCLEANFILES = $(idl_DATA:.idl=.cc) $(idl_HEADERS) $(METASOURCES)

idl_DATA = kohtml.idl
idl_HEADERS =  kohtml.h

kdelnk_DATA = kohtml_part.kdelnk
kdelnkdir = $(kde_datadir)/koffice/partlnk

pics_DATA = kohtml.xpm
picsdir = $(kde_icondir)

SUBDIRS = toolbar

####### Implicit rules

####### Build rules

install-exec-local:
	$(INSTALL_SCRIPT) $(srcdir)/kohtml $(bindir)/kohtml

uninstall-local:
	-rm -f $(bindir)/kohtml

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot  *.cc && mv messages.po ../po/kohtml.pot

kohtml.h: kohtml.idl
	rm -f kohtml.h
	$(IDL) --query-server-for-narrow --relative-paths -I$(top_srcdir)/lib/kofficecore -I$(top_srcdir)/lib/store -I$(idldir) $(srcdir)/kohtml.idl

BUILT_SOURCES = kohtml.h

