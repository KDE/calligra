# $Id$

SUBDIRS = core ui pics

INCLUDES  = $(MICO_INCLUDES)  -I$(srcdir)/ui -I$(srcdir)/core \
	$(KOFFICE_INCLUDES) $(all_includes) 
LDFLAGS =   $(all_libraries) $(MICO_LDFLAGS) $(KDE_RPATH)
LDADD =  $(LIB_KOFFICEUI)

####### Files

bin_PROGRAMS = kchart

kchart_SOURCES = kchart_main.cc kchart_doc.cc kchart_view.cc kchart.idl kchart_shell.cc chart.idl \
		sheet.cc sheetdlg.cc table.cc parser.cc
noinst_HEADERS = kchart_main.h kchart_doc.h kchart_view.h kchart.h kchart_shell.h chart.h \
		sheet.h sheetdlg.h table.h parser.h
METASOURCES =	kchart_main.moc kchart_view.moc kchart_doc.moc kchart_shell.moc \
		sheet.moc sheetdlg.moc table.moc

idl_DATA = kchart.idl

kdelnk_DATA = kchart_part.kdelnk
kdelnkdir = $(kde_datadir)/koffice/partlnk

####### Implicit rules

####### Build rules

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot *.cc */*.cpp && mv messages.po ../po/kchart.pot

IDL_FLAGS = -I$(idldir) -I$(interfacedir)

