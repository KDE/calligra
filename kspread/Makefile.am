
INCLUDES  = $(PYTHONINC) $(MICO_INCLUDES) -I../lib/store -I../lib/koml -I../lib/kofficecore -I../lib/kofficeui -I../lib/interface $(all_includes) -I$(idldir)
LDFLAGS   = $(all_libraries) $(PYTHONLIB) $(MICO_LDFLAGS) $(KDE_RPATH)
LDADD = ../lib/kofficeui/libkofficeui.la ../lib/kofficecore/libkofficecore.la ../lib/koml/libkoml.la ../lib/store/libkstore.la -lkparts -lkpartsui -lkom -lkfile -lkfm -lkdeui -lkdeutil -lkdecore -lqt $(LIBMICO) $(LIBPYTHON) -lXext -lX11 $(LIBSOCKET) $(LIBCRYPT) 

####### Files

bin_PROGRAMS = kspread.bin

kspread_bin_SOURCES = kmetaedit.cc kspread_emacs.cc kspread_main.cc \
	kspread_shell.cc kspread_doc.cc \
	kspread.cc kspread_undo.cc kspread_view.cc kspread_tabbar.cc \
	kspread_python.cc kspread_autofill.cc \
	kspread_layout.cc kspread_map.cc kspread_dlg_layout.cc \
	kspread_dlg_scripts.cc kspread_cell.cc kspread_table.cc \
	chart.cc kspread_calc.cc y.tab.c lex.yy.c dep.tab.c lex.dep.c \
	kspread_dlg_cons.cc

noinst_HEADERS = kmetaedit.h kspread_emacs.h kspread_main.cc \
	kspread_shell.h kspread_doc.h \
	kspread.h kspread_undo.h kspread_view.h kspread_tabbar.h \
	kspread_python.h kspread_autofill.h \
	kspread_layout.h kspread_map.h kspread_dlg_layout.h \
	kspread_dlg_scripts.h kspread_table.h kspread_cell.h \
	chart.h y.tab.h dep.tab.h kspread_calc.h kspread_dlg_cons.h

SRCMETA = kspread_main.moc kspread_shell.moc kspread_doc.moc \
	kspread_view.moc kspread_tabbar.moc \
	kspread_table.moc kspread_dlg_layout.moc \
	kspread_dlg_scripts.moc kspread_part.moc kspread_dlg_cons.moc

idl_DATA = kspread.idl
idl_HEADERS = kspread.h

DISTCLEANFILES = $(idl_DATA:.idl=.cc) $(idl_HEADERS) $(SRCMETA) chart.h chart.cc

kdelnk_DATA = kspread_part.kdelnk
kdelnkdir = $(kde_datadir)/koffice/partlnk

SUBDIRS = pics scripts

####### Implicit rules

SUFFIXES = .h .moc

.h.moc: 
	$(MOC) $< -o $@

####### Build rules

parser:
	flex lex.l
	yacc -d yacc.y
	flex -Pdep lex.dep.l
	bison -p dep -b dep -d yacc.dep.y 

IDL_FLAGS = --query-server-for-narrow --relative-paths -I../lib/kofficecore -I../lib/store -I../lib/interfaces -I$(idldir)

install-exec-local:
	$(INSTALL_SCRIPT) kspread $(bindir)/kspread

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot -x $(includedir)/corba.pot  *.cc && mv messages.po ../po/kspread.pot

kspread.h: kspread.idl
	$(IDL) $(IDL_FLAGS) kspread.idl

chart.h: ../lib/interfaces/chart.idl
	$(IDL) $(IDL_FLAGS) ../lib/interfaces/chart.idl

kspread.cc: kspread.h
kspread_dlg_cons.cc: kspread_dlg_cons.moc
kspread_dlg_layout.cc: kspread_dlg_layout.moc
kspread_dlg_paper.cc: kspread_dlg_paper.moc
kspread_dlg_paper_border.cc: kspread_dlg_paper_border.moc
kspread_dlg_paper_headings.cc: kspread_dlg_paper_headings.moc
kspread_dlg_print.cc: kspread_dlg_print.moc
kspread_dlg_scripts.cc: kspread_dlg_scripts.moc
kspread_doc.cc: kspread.h kspread_doc.moc
kspread_main.cc: kspread.h kspread_table.h kspread_main.moc
kspread_part.cc: kspread.h kspread_part.moc
kspread_shell.cc: kspread.h kspread_shell.moc
kspread_tabbar.cc: kspread.h kspread_tabbar.moc
kspread_table.cc: kspread.h kspread_table.moc
kspread_view.cc: kspread.h kspread_view.moc kspread_table.h chart.h
kspread_table.h: chart.h

