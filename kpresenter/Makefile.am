
INCLUDES = $(MICO_INCLUDES) -I../lib/kofficecore -I../lib/kofficeui \
	-I../lib/koml $(all_includes) -I$(idldir)
LDFLAGS	= $(all_libraries) $(MICO_LDFLAGS) $(KDE_RPATH)
LDADD = autoformEdit/atfinterpreter.o autoformEdit/afchoose.o \
	../lib/kofficeui/libkofficeui.la \
	../lib/kofficecore/libkofficecore.la ../lib/koml/libkoml.la \
	-lop -lkfile -lkfm -lkdeui -lkdecore $(LIBMICO) -lqt -lXext -lX11 \
	-lkom  -ljpeg $(LIBSOCKET) -ldl

####### Files

bin_PROGRAMS = kpresenter.bin

kpresenter_bin_SOURCES = kpresenter_main.cc kpresenter_doc.cc \
	kpresenter_view.cc kpresenter_shell.cc kpresenter.cc \
	backdia.cc page.cc ktextobject.cc styledia.cc \
	jpeg.cc formats.cc qwmf.cc kenumlistdia.cc kcharselect.cc \
	kcharselectdia.cc optiondia.cc pgconfdia.cc effectdia.cc \
	kpresenter_utils.cc rotatedia.cc ksearchdialogs.cc \
	shadowdia.cc presstructview.cc \
	\
	kpobject.cc kplineobject.cc kprectobject.cc kpellipseobject.cc \
	kpautoformobject.cc kpclipartobject.cc kptextobject.cc \
	kppixmapobject.cc \
	kpbackground.cc kppixmap.cc kpclipart.cc kpgradient.cc\
	kppixmapcollection.cc kpgradientcollection.cc \
	command.cc movecmd.cc resizecmd.cc insertcmd.cc deletecmd.cc \
	setoptionscmd.cc chgclipcmd.cc chgpixcmd.cc setbackcmd.cc \
	commandhistory.cc

noinst_HEADERS = kpresenter_main.h kpresenter_doc.h kpresenter_view.h \
		kpresenter_shell.h kpresenter.h \
		backdia.h page.h ktextobject.h styledia.h \
		jpeg.h formats.h qwmf.h global.h \
		kenumlistdia.h kcharselect.h kcharselectdia.h optiondia.h \
		pgconfdia.h effectdia.h kpresenter_utils.h \
		rotatedia.h ksearchdialogs.h shadowdia.h presstructview.h \
		\
		kpobject.h kplineobject.h kprectobject.h kpellipseobject.h \
		kpautoformobject.h kpclipartobject.h kptextobject.h \
		kppixmapobject.h \
		kpbackground.h kppixmap.h kpclipart.h kpgradient.h \
		kppixmapcollection.h kpgradientcollection.h \
		command.h movecmd.h resizecmd.h insertcmd.h deletecmd.h \
		setoptionscmd.h chgclipcmd.h chgpixcmd.h setbackcmd.h \
		commandhistory.h

SRCMETA =	kpresenter_main.moc kpresenter_doc.moc kpresenter_view.moc \
		kpresenter_shell.moc \
		page.moc ktextobject.moc backdia.moc styledia.moc \
		kenumlistdia.moc kcharselect.moc kcharselectdia.moc \
		optiondia.moc pgconfdia.moc effectdia.moc \
		rotatedia.moc ksearchdialogs.moc shadowdia.moc presstructview.moc \
		\
		kpobject.moc kplineobject.moc kprectobject.moc kpellipseobject.moc \
		kpautoformobject.moc kpclipartobject.moc kptextobject.moc \
		kppixmapobject.moc
		kpbackground.moc kppixmap.moc kpclipart.moc kpgradient.moc\
		kppixmapcollection.moc kpgradientcollection.moc \
		command.moc movecmd.moc resizecmd.moc insertcmd.moc deletecmd.moc \
		setoptionscmd.moc chgclipcmd.moc chgpixcmd.moc setbackcmd.moc \
		commandhistory.moc

idl_DATA =	kpresenter.idl
idl_HEADERS =    kpresenter.h

DISTCLEANFILES = $(idl_DATA:.idl=.cc) $(idl_HEADERS) $(SRCMETA)

kdelnk_DATA = kpresenter.kdelnk
kdelnkdir = $(kde_datadir)/koffice/partlnk

SUBDIRS = autoformEdit autoforms pics toolbar templates

####### Implicit rules

SUFFIXES = .h .moc 

.h.moc:
	$(MOC) $< -o $@      

####### Build rules

install-exec-local:
	$(INSTALL_SCRIPT) kpresenter $(bindir)/kpresenter

messages:
	$(XGETTEXT) -C -ki18n -ktranslate -x $(includedir)/kde.pot -x $(includedir)/corba.pot  *.cc */*.cc  && mv messages.po ../po/kpresenter.pot

kpresenter.h: kpresenter.idl
	rm -f kpresenter.h
	$(IDL) --query-server-for-narrow -I$(idldir) kpresenter.idl

kpresenter.cc: kpresenter.h
kpresenter_doc.cc: kpresenter_doc.moc
kpresenter_main.cc: kpresenter.h kpresenter_main.moc
kpresenter_shell.cc: kpresenter.h kpresenter_shell.moc
kpresenter_view.cc: kpresenter.h kpresenter_view.moc
backdia.cc: backdia.moc
page.cc: page.moc kpresenter.h
ktextobject.cc: ktextobject.moc
styledia.cc: styledia.moc
kenumlistdia.cc: kenumlistdia.moc
kcharselect.cc: kcharselect.moc
kcharselectdia.cc: kcharselectdia.moc
optiondia.cc: optiondia.moc
pgconfdia.cc: pgconfdia.moc
effectdia.cc: effectdia.moc
rotatedia.cc: rotatedia.moc
ksearchdialogs.cc: ksearchdialogs.moc
shadowdia.cc: shadowdia.moc
presstructview.cc: presstructview.moc

kpobject.cc: kpobject.moc
kplineobject.cc: kplineobject.moc
kprectobject.cc: kprectobject.moc
kpellipseobject.cc: kpellipseobject.moc
kpautoformobject.cc: kpautoformobject.moc
kpclipartobject.cc: kpclipartobject.moc
kptextobject.cc: kptextobject.moc
kppixmapobject.cc: kppixmapobject.moc
kpbackground.cc: kpbackground.moc
kppixmap.cc: kppixmap.moc
kpclipart.cc: kpclipart.moc
kpgradient.cc: kpgradient.moc
kppixmapcollection.cc: kppixmapcollection.moc
kpgradientcollection.cc: kpgradientcollection.moc

command.cc: command.moc
movecmd.cc: movecmd.moc
resizecmd.cc: resizecmd.moc
insertcmd.cc: insertcmd.moc
deletecmd.cc: deletecmd.moc
setoptionscmd.cc: setoptionscmd.moc
chgclipcmd.cc: chgclipcmd.moc
chgpixcmd.cc: chgpixcmd.moc
setbackcmd.cc: setbackcmd.moc
commandhistory.cc: commandhistory.moc