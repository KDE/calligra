
SUBDIRS = pics

INCLUDES  = $(PYTHONINC) $(MICO_INCLUDES) -I../lib/store -I../lib/koml -I../lib/kofficecore -I../lib/kofficeui -I../lib/interface $(all_includes) -I$(idldir)
LDFLAGS   = $(all_libraries) $(PYTHONLIB) $(MICO_LDFLAGS) $(KDE_RPATH)
LDADD = ../lib/kofficeui/libkofficeui.la \
	../lib/kofficecore/libkofficecore.la ../lib/koml/libkoml.la \
	../lib/store/libkstore.la -lkparts -lkpartsui \
	-lkom -lkio -lkdeutil $(LIBMICO) $(LIBPYTHON) $(LIB_KFILE) 

####### Files

bin_PROGRAMS = kdiagramm.bin

kdiagramm_bin_SOURCES = kdiagramm_main.cc kdiagramm_shell.cc kdiagramm_doc.cc \
			kdiagramm.cc kdiagramm_view.cc koDiagramm.cc chart.cc \
			sheet.cc table.cc parser.cc sheetdlg.cc

noinst_HEADERS = kdiagramm_main.h kdiagramm_shell.h kdiagramm_doc.h \
	         kdiagramm.h kdiagramm_view.h chart.h \
		 sheet.h table.h parser.h sheetdlg.h

SRCMETA = kdiagramm_main.moc kdiagramm_shell.moc kdiagramm_doc.moc \
	kdiagramm_view.moc koDiagramm.moc table.moc sheet.moc sheetdlg.moc

idl_DATA = kdiagramm.idl
idl_HEADERS = kdiagramm.h

DISTCLEANFILES = $(idl_DATA:.idl=.cc) $(idl_HEADERS) $(SRCMETA) chart.h chart.cc

kdelnk_DATA = kdiagramm_part.kdelnk
kdelnkdir = $(kde_datadir)/koffice/partlnk

####### Implicit rules

SUFFIXES = .h .moc

.h.moc: 
	$(MOC) $< -o $@

####### Build rules

IDL_FLAGS = --query-server-for-narrow --relative-paths -I../lib/kofficecore -I../lib/store -I../lib/interfaces -I$(idldir)

install-exec-local:
	$(INSTALL_SCRIPT) kdiagramm $(bindir)/kdiagramm

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot -x $(includedir)/corba.pot  *.cc && mv messages.po ../po/kdiagramm.pot

kdiagramm.h: kdiagramm.idl
	$(IDL) $(IDL_FLAGS) kdiagramm.idl

chart.h: ../lib/interfaces/chart.idl
	$(IDL) $(IDL_FLAGS) ../lib/interfaces/chart.idl

chart.cc: chart.h
table.cc: table.h table.moc
sheet.cc: sheet.h sheet.moc
sheetdlg.cc: sheetdlg.h sheetdlg.moc
parser.cc: parser.h
kdiagramm.cc: kdiagramm.h
kdiagramm_doc.cc: kdiagramm.h kdiagramm_doc.moc
kdiagramm_main.cc: kdiagramm.h kdiagramm_main.moc
kdiagramm_shell.cc: kdiagramm.h kdiagramm_shell.moc
kdiagramm_view.cc: kdiagramm.h kdiagramm_view.moc
koDiagramm.cc: koDiagramm.h koDiagramm.moc
