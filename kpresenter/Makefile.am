
INCLUDES=	-I../lib/kofficeui $(all_includes) -I$(idldir)
LDFLAGS	=	$(all_libraries) 
LDADD = ../lib/kofficeui/libkofficeui.la \
	../lib/kofficecore/libkofficecore.la ../lib/koml/libkoml.la \
	-lkdeui -lkdecore -lkfm $(LIBMICO) -lXext -lX11 -lkom -lop \
	-ljpeg -lkfile -lqt -lkoml

####### Files

bin_PROGRAMS = kpresenter
kpresenter_SOURCES = kpresenter_main.cc kpresenter_doc.cc kpresenter_view.cc \
	kpresenter.cc kpresenter_shell.cc graphobj.cc atfinterpreter.cc\
	backdia.cc page.cc ktextobject.cc afchoose.cc styledia.cc \
	jpeg.cc formats.cc qwmf.cc kenumlistdia.cc kcharselect.cc \
	kcharselectdia.cc 

noinst_HEADERS = kpresenter_main.h kpresenter_doc.h kpresenter_view.h \
		kpresenter.h kpresenter_shell.h graphobj.h atfinterpreter.h \
		backdia.h page.h ktextobject.h afchoose.h styledia.h \
		jpeg.h formats.h qwmf.h global.h \
		kenumlistdia.h kcharselect.h kcharselectdia.h 

SRCMETA =	kpresenter_main.moc kpresenter_doc.moc kpresenter_view.moc \
		kpresenter_shell.moc graphobj.moc atfinterpreter.moc \
		page.moc ktextobject.mocbackdia.moc afchoose.moc styledia.moc \
		kenumlistdia.moc kcharselect.moc kcharselectdia.moc 

idl_DATA =	kpresenter.idl
idl_HEADER =    kpresenter.h

kdelnkdir = $(kde_datadir)/koffice/partlnk
kdelnk_DATA = kpresenter.kdelnk

SUBDIRS = autoformEdit autoforms pics toolbar 

####### Implicit rules

SUFFIXES = .h .moc 

.h.moc:
	$(MOC) $< -o $@      

idl: kpresenter.idl
	idl -I$(idldir) kpresenter.idl

