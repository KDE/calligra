# $Id$

SUBDIRS = core ui pics

INCLUDES  = $(MICO_INCLUDES)  -I$(srcdir)/ui -I$(srcdir)/core \
	$(KOFFICE_INCLUDES) $(all_includes) -I$(idldir)
LDFLAGS =   $(all_libraries) $(MICO_LDFLAGS) $(KDE_RPATH)
LDADD =  $(LIB_KOFFICEUI)

####### Files

bin_PROGRAMS = kchart.bin

kchart_bin_SOURCES = kchart_main.cc kchart_doc.cc kchart_view.cc kchart.cc kchart_shell.cc chart.cc \
		sheet.cc sheetdlg.cc table.cc parser.cc
noinst_HEADERS = kchart_main.h kchart_doc.h kchart_view.h kchart.h kchart_shell.h chart.h \
		sheet.h sheetdlg.h table.h parser.h
METASOURCES =	kchart_main.moc kchart_view.moc kchart_doc.moc kchart_shell.moc \
		sheet.moc sheetdlg.moc table.moc

DISTCLEANFILES = $(idl_HEADERS:.h=.cc) $(idl_HEADERS) $(METASOURCES)

IDL_INCLUDES = -I$(idldir) -I$(interfacedir)

idl_DATA = kchart.idl
idl_HEADERS = kchart.h chart.h

kdelnk_DATA = kchart_part.kdelnk
kdelnkdir = $(kde_datadir)/koffice/partlnk

####### Implicit rules

####### Build rules

install-exec-local:
	$(INSTALL_SCRIPT) $(srcdir)/kchart $(bindir)/kchart

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot *.cc */*.cpp && mv messages.po ../po/kchart.pot

kchart.h: kchart.idl
	@echo $(IDL) --query-server-for-narrow $(IDL_INCLUDES) kchart.idl ;\
	rm -f kchart.h kchart.cc ;\
	$(IDL) --relative-paths --query-server-for-narrow $(IDL_INCLUDES) $(srcdir)/kchart.idl

chart.h: $(interfacedir)/chart.idl
	$(IDL) --query-server-for-narrow -I$(idldir) $(interfacedir)/chart.idl

kchart.cc: kchart.h
kchart_view.h: kchart.h
kchart_shell.h: kchart.h
kchart_main.cc:  kchart.h chart.h
kchart_main.cc: kchart_view.h kchart.h
kchart.h: chart.h
sheet.cc: sheet.h 
sheetdlg.cc: sheetdlg.h 
parser.cc: parser.h
table.cc: table.h 
