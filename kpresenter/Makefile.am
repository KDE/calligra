
INCLUDES = $(MICO_INCLUDES) $(KOFFICE_INCLUDES) $(all_includes) -I$(idldir)
LDFLAGS	= $(all_libraries) $(MICO_LDFLAGS) $(KDE_RPATH)
LDADD = autoformEdit/libaf.a ../lib/kofficeui/libkofficeui.la $(LIBQIMGIO) $(LIB_KIMGIO)

####### Files

bin_PROGRAMS = kpresenter

kpresenter_SOURCES = kpresenter_view.cc kpresenter_main.cc \
	kpresenter_doc.cc kpresenter_shell.cc kpresenter.idl \
	backdia.cc page.cc ktextobject.cc styledia.cc \
	qwmf.cc kenumlistdia.cc \
	kcharselectdia.cc optiondia.cc pgconfdia.cc effectdia.cc \
	kpresenter_utils.cc rotatedia.cc ksearchdialogs.cc \
	shadowdia.cc presstructview.cc delpagedia.cc inspagedia.cc \
	confpiedia.cc spacingdia.cc confrectdia.cc \
	kpobject.cc kplineobject.cc kprectobject.cc kpellipseobject.cc \
	kpautoformobject.cc kpclipartobject.cc kptextobject.cc \
	kppixmapobject.cc kppieobject.cc kppartobject.cc \
	kpbackground.cc kpclipartcollection.cc kpgradient.cc\
	kppixmapcollection.cc kpgradientcollection.cc \
	command.cc movecmd.cc resizecmd.cc insertcmd.cc deletecmd.cc \
	setoptionscmd.cc chgclipcmd.cc chgpixcmd.cc setbackcmd.cc \
	effectcmd.cc pgconfcmd.cc pglayoutcmd.cc shadowcmd.cc \
	rotatecmd.cc lowraicmd.cc pievaluecmd.cc rectvaluecmd.cc penbrushcmd.cc \
	commandhistory.cc preview.cc gotopage.cc webpresentation.cc \
	footer_header.cc

noinst_HEADERS = kpresenter_main.h kpresenter_doc.h kpresenter_view.h \
		kpresenter_shell.h kpresenter.h \
		backdia.h page.h ktextobject.h styledia.h \
		qwmf.h global.h \
		kenumlistdia.h kcharselectdia.h optiondia.h \
		pgconfdia.h effectdia.h kpresenter_utils.h \
		rotatedia.h ksearchdialogs.h shadowdia.h presstructview.h \
		delpagedia.h inspagedia.h confpiedia.h spacingdia.h confrectdia.h \
		kpobject.h kplineobject.h kprectobject.h kpellipseobject.h \
		kpautoformobject.h kpclipartobject.h kptextobject.h \
		kppixmapobject.h kppieobject.h kppartobject.h \
		kpbackground.h kpclipartcollection.h kpgradient.h \
		kppixmapcollection.h kpgradientcollection.h \
		command.h movecmd.h resizecmd.h insertcmd.h deletecmd.h \
		setoptionscmd.h chgclipcmd.h chgpixcmd.h setbackcmd.h \
		effectcmd.h pgconfcmd.h pglayoutcmd.h shadowcmd.h \
		rotatecmd.h lowraicmd.h pievaluecmd.h rectvaluecmd.h penbrushcmd.cc \
		commandhistory.h preview.h gotopage.h webpresentation.h \
		footer_header.h

METASOURCES =	kpresenter_main.moc kpresenter_doc.moc kpresenter_view.moc \
		kpresenter_shell.moc \
		page.moc ktextobject.moc backdia.moc styledia.moc \
		kenumlistdia.moc kcharselectdia.moc \
		optiondia.moc pgconfdia.moc effectdia.moc \
		rotatedia.moc ksearchdialogs.moc shadowdia.moc \
		presstructview.moc delpagedia.moc inspagedia.moc confpiedia.moc \
		spacingdia.moc confrectdia.moc \
		commandhistory.moc gotopage.moc webpresentation.moc \
		footer_header.moc

idl_DATA =	kpresenter.idl

kdelnk_DATA = kpresenter.desktop
kdelnkdir = $(kde_appsdir)/KOffice

SUBDIRS = autoformEdit . autoforms pics toolbar templates dtd slideshow

messages:
	$(XGETTEXT) -C -ki18n -ktranslate -x $(includedir)/kde.pot */*.cc *.cc *.h && mv messages.po ../po/kpresenter.pot

IDL_FLAGS = --query-server-for-narrow --relative-paths -I$(top_srcdir)/lib/kofficecore -I$(top_srcdir)/lib/store -I$(idldir) 

