INCLUDES  = $(PYTHONINC) $(MICO_INCLUDES) $(KOFFICE_INCLUDES) -I$(interfacedir) $(all_includes) -I$(idldir)
LDFLAGS   = $(all_libraries) $(KDE_RPATH) $(PYTHONLIB)
LDADD = ../lib/kofficeui/libkofficeui.la $(LIBPYTHON)

####### Files

bin_PROGRAMS = kspread.bin

kspread_bin_SOURCES = kmetaedit.cc kspread_emacs.cc kspread_main.cc \
	kspread_shell.cc kspread_doc.cc \
	kspread.cc kspread_undo.cc kspread_view.cc kspread_tabbar.cc \
	kspread_python.cc kspread_autofill.cc \
	kspread_layout.cc kspread_map.cc kspread_dlg_layout.cc \
	kspread_dlg_scripts.cc kspread_cell.cc kspread_table.cc \
	chart.cc kspread_calc.cc y.tab.c lex.yy.c dep.tab.c lex.dep.c \
	kspread_dlg_cons.cc datatools.cc kspread_util.cc

noinst_HEADERS = kmetaedit.h kspread_emacs.h kspread_main.cc \
	kspread_shell.h kspread_doc.h \
	kspread.h kspread_undo.h kspread_view.h kspread_tabbar.h \
	kspread_python.h kspread_autofill.h \
	kspread_layout.h kspread_map.h kspread_dlg_layout.h \
	kspread_dlg_scripts.h kspread_table.h kspread_cell.h \
	chart.h y.tab.h dep.tab.h kspread_calc.h kspread_dlg_cons.h \
	datatools.h kspread_util.h

METASOURCES = kspread_main.moc kspread_shell.moc kspread_doc.moc \
	kspread_view.moc kspread_tabbar.moc \
	kspread_table.moc kspread_dlg_layout.moc kspread_cell.moc \
	kspread_dlg_scripts.moc kspread_part.moc kspread_dlg_cons.moc

idl_DATA = kspread.idl
idl_HEADERS = kspread.h chart.h

DISTCLEANFILES = $(idl_HEADERS:.h=.cc) $(idl_HEADERS) $(METASOURCES)

kdelnk_DATA = kspread_part.kdelnk
kdelnkdir = $(kde_datadir)/koffice/partlnk

kdemime_DATA = x-kspread.kdelnk
kdemimedir = $(kde_mimedir)/application

py_DATA = KSpread.py KOffice.py KOStore.py OpenPartsUI.py OpenParts.py KOM.py
pydir = $(kde_includes)/python

SUBDIRS =  . pics scripts dtd filters toolbar

####### Implicit rules

####### Build rules

parser:
	flex lex.l
	yacc -d yacc.y
	flex -Pdep lex.dep.l
	bison -p dep -b dep -d yacc.dep.y 

IDL_FLAGS = --query-server-for-narrow --relative-paths -I$(top_srcdir)/lib/kofficecore -I$(top_srcdir)/lib/store -I$(top_srcdir)/lib/interfaces -I$(idldir)

install-exec-local:
	$(INSTALL_SCRIPT) $(srcdir)/kspread $(bindir)/kspread

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot -x $(includedir)/corba.pot  *.cc && mv messages.po ../po/kspread.pot

kspread.h: kspread.idl $(top_srcdir)/lib/kofficecore/koffice.idl
	$(IDL) $(IDL_FLAGS) $(srcdir)/kspread.idl

chart.h: $(interfacedir)/chart.idl
	$(IDL) $(IDL_FLAGS) $(interfacedir)/chart.idl

datatools.h: $(idldir)/datatools.idl
	$(IDL) $(IDL_FLAGS) $(idldir)/datatools.idl

datatools.cc: datatools.h
kspread.cc: kspread.h
kspread_doc.cc: kspread.h 
kspread_main.cc: kspread.h kspread_table.h 
kspread_part.cc: kspread.h 
kspread_shell.cc: kspread.h 
kspread_tabbar.cc: kspread.h 
kspread_table.cc: kspread.h 
kspread_view.cc: kspread.h  kspread_table.h chart.h
kspread_table.h: chart.h
kspread_util.h: kspread.h
KOM.py: KSpread.py
KOStore.py: KSpread.py
KOffice.py: KSpread.py
OpenParts.py: KSpread.py
OpenPartsUI.py: KSpread.py
KSpread.py: kspread.idl $(kde_bindir)/pyidl $(top_srcdir)/lib/kofficecore/koffice.idl
	$(kde_bindir)/pyidl "-I$(idldir) -I$(top_srcdir)/lib/kofficecore -I$(top_srcdir)/lib/store -I$(interfacedir) $(srcdir)/kspread.idl"

