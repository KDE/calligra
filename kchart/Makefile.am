# $Id$

SUBDIRS = core ui pics

INCLUDES  = $(MICO_INCLUDES)  -I./core  -I../lib/koml \
	-I../lib/kofficecore -I../lib/kofficeui $(all_includes) -I$(idldir)
LDFLAGS =   $(all_libraries) $(MICO_LDFLAGS) $(KDE_RPATH)
LDADD =  ../lib/kofficeui/libkofficeui.la \
	../lib/kofficecore/libkofficecore.la \
	./core/libkchartcore.la ./ui/libkchartui.la \
	../lib/koml/libkoml.la -lkfile -lkfm -lkom -lop -lkdeui -lkdecore \
	-lqt $(LIBMICO) -lXext -lX11 $(LIBSOCKET)

####### Files

bin_PROGRAMS = kchart.bin

kchart_bin_SOURCES = kchart_main.cc kchart_doc.cc kchart_view.cc kchart.cc kchart_shell.cc chart.cc
noinst_HEADERS = kchart_main.h kchart_doc.h kchart_view.h kchart.h kchart_shell.h chart.h
SRCMETA =	kchart_main.moc kchart_view.moc kchart_doc.moc kchart_shell.moc

interfacedir = ../lib/interfaces
IDL_INCLUDES = -I$(idldir) -I$(interfacedir)

idl_DATA = kchart.idl
idl_HEADER = kchart.h

kdelnk_DATA = kchart_part.kdelnk
kdelnkdir = $(kde_datadir)/koffice/partlnk

####### Implicit rules

SUFFIXES = .h .moc

.h.moc:
	$(MOC) $< -o $@      

####### Build rules

install-exec-local:
	$(INSTALL_SCRIPT) kchart $(bindir)/kchart

kchart.h: kchart.idl
	@echo $(IDL) --query-server-for-narrow $(IDL_INCLUDES) kchart.idl ;\
	-rm -f kchart.h kchart.cc ;\
	$(IDL) --query-server-for-narrow $(IDL_INCLUDES) kchart.idl ;\
	sed -e "s,^#include \".*/\(.*\)\",#include <\1>," kchart.h > kchart.h.new && mv kchart.h.new kchart.h

chart.h: ../lib/interfaces/chart.idl
	$(IDL) --query-server-for-narrow -I$(idldir) $<

kchart.cc: kchart.h
kchart_view.h: kchart.h
kchart_shell.h: kchart.h
kchart_doc.cc: kchart_doc.moc
kchart_main.cc: kchart_main.moc kchart.h chart.h
kchart_shell.cc: kchart_shell.moc
kchart_view.cc: kchart_view.moc
kchart_main.cc: kchart_view.h kchart.h
kchart.h: chart.h
