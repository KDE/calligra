
INCLUDES=	-I../lib/kofficeui $(all_includes) -I$(idldir)
LDFLAGS	=	$(all_libraries) 
LDADD = autoformEdit/atfinterpreter.o autoformEdit/afchoose.o \
	../lib/kofficeui/libkofficeui.la \
	../lib/kofficecore/libkofficecore.la ../lib/koml/libkoml.la \
	-lkdeui -lkdecore -lkfm $(LIBMICO) -lXext -lX11 -lkom -lop \
	-ljpeg -lkfile -lqt -lkoml

####### Files

SUBDIRS = autoformEdit autoforms pics toolbar 

bin_PROGRAMS = kpresenter
kpresenter_SOURCES = kpresenter_main.cc kpresenter_doc.cc kpresenter_view.cc \
	kpresenter.cc kpresenter_shell.cc graphobj.cc \
	backdia.cc page.cc ktextobject.cc styledia.cc \
	jpeg.cc formats.cc qwmf.cc kenumlistdia.cc kcharselect.cc \
	kcharselectdia.cc 

noinst_HEADERS = kpresenter_main.h kpresenter_doc.h kpresenter_view.h \
		kpresenter.h kpresenter_shell.h graphobj.h \
		backdia.h page.h ktextobject.h styledia.h \
		jpeg.h formats.h qwmf.h global.h \
		kenumlistdia.h kcharselect.h kcharselectdia.h 

SRCMETA =	kpresenter_main.moc kpresenter_doc.moc kpresenter_view.moc \
		kpresenter_shell.moc graphobj.moc moc \
		page.moc ktextobject.mocbackdia.moc styledia.moc \
		kenumlistdia.moc kcharselect.moc kcharselectdia.moc 

idl_DATA =	kpresenter.idl
idl_HEADER =    kpresenter.h

kdelnkdir = $(kde_datadir)/koffice/partlnk
kdelnk_DATA = kpresenter.kdelnk

####### Implicit rules

SUFFIXES = .h .moc 

.h.moc:
	$(MOC) $< -o $@      

idl: kpresenter.idl
	idl -I$(idldir) kpresenter.idl


kpresenter_main.cc: kpresenter_main.moc
kpresenter_doc.cc: kpresenter_doc.moc
kpresenter_view.cc: kpresenter_view.moc
kpresenter_shell.cc: kpresenter_shell.moc
graphobj.cc: graphobj.moc
backdia.cc: backdia.moc
page.cc: page.moc
ktextobject.cc: ktextobject.moc
styledia.cc: styledia.moc
kenumlistdia.cc: kenumlistdia.moc
kcharselect.cc: kcharselect.moc
kcharselectdia.cc: kcharselectdia.moc
