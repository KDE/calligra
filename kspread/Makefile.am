INCLUDES  = $(MICO_INCLUDES) $(KOFFICE_INCLUDES) -I$(interfacedir) $(all_includes) -I$(top_srcdir)/lib/kscript -I$(top_builddir)/lib/kscript
LDFLAGS   = $(all_libraries) $(KDE_RPATH)
LDADD = ../lib/kscript/libkscript.la ../lib/kofficeui/libkofficeui.la -lkformula

####### Files

bin_PROGRAMS = kspread

kspread_SOURCES = kspread.idl kspread_canvas.cc kspread_main.cc \
	kspread_shell.cc kspread_doc.cc \
	kspread_undo.cc kspread_view.cc kspread_tabbar.cc \
	kspread_autofill.cc kspread_interpreter.cc \
	kspread_layout.cc kspread_map.cc kspread_dlg_layout.cc \
	kspread_dlg_scripts.cc kspread_cell.cc kspread_table.cc \
	chart.idl kspread_calc.cc y.tab.c lex.yy.c dep.tab.c lex.dep.c \
	kspread_dlg_cons.cc datatools.idl kspread_util.cc \
	kspread_corba_util.cc kspread_editors.cc kspread_dlg_tabname.cc \
	kspread_dlg_formula.cc kspread_dlg_assistant.cc \
	kspread_dlg_special.cc kspread_dlg_goto.cc  kspread_dlg_replace.cc \
	kspread_dlg_sort.cc   kspread_dlg_create.cc   kspread_dlg_anchor.cc kspread_dlg_assistant2.cc

noinst_HEADERS = kspread_main.h \
	kspread_shell.h kspread_doc.h \
	kspread.h kspread_undo.h kspread_view.h kspread_tabbar.h \
	kspread_autofill.h kspread_interpreter.h \
	kspread_layout.h kspread_map.h kspread_dlg_layout.h \
	kspread_dlg_scripts.h kspread_table.h kspread_cell.h \
	chart.h y.tab.h dep.tab.h kspread_calc.h kspread_dlg_cons.h \
	datatools.h kspread_util.h kspread_corba_util.h kspread_canvas.h \
	kspread_editors.h kspread_dlg_tabname.h kspread_dlg_formula.h \
	kspread_dlg_assistant.h kspread_dlg_special.h kspread_dlg_goto.h kspread_dlg_replace.h \
	kspread_dlg_sort.h kspread_dlg_create.h kspread_dlg_anchor.h kspread_dlg_assistant2.h

METASOURCES = kspread_main.moc kspread_shell.moc kspread_doc.moc \
	kspread_view.moc kspread_tabbar.moc kspread_canvas.moc \
	kspread_table.moc kspread_dlg_layout.moc kspread_cell.moc \
	kspread_dlg_scripts.moc kspread_part.moc kspread_dlg_cons.moc \
	kspread_editors.moc kspread_dlg_tabname.moc kspread_dlg_formula.moc \
	kspread_dlg_assistant.moc kspread_dlg_special.moc kspread_dlg_goto.moc kspread_dlg_replace.moc \
	kspread_dlg_sort.moc  kspread_dlg_create.moc kspread_dlg_anchor.moc kspread_dlg_assistant2.moc

idl_DATA = kspread.idl

# DISTCLEANFILES = KOStore.py KOffice.py KSpread.py OpenPartsUI.py OpenParts.py KOM.py 

kdelnk_DATA = kspread.desktop
kdelnkdir = $(kde_appsdir)/KOffice

kdemime_DATA = x-kspread.kdelnk
kdemimedir = $(kde_mimedir)/application

SUBDIRS =  . pics dtd toolbar filters

chart_IDLDIR = $(interfacedir)

datatools_IDLDIR = $(idldir)

parser:
	flex lex.l
	yacc -d yacc.y
	flex -Pdep lex.dep.l
	bison -p dep -b dep -d yacc.dep.y 

IDL_FLAGS = --relative-paths --any -I$(top_srcdir)/lib/kofficecore -I$(top_srcdir)/lib/store -I$(top_srcdir)/lib/interfaces -I$(idldir)

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot *.cc && mv messages.po ../po/kspread.pot

#KOM.py: KSpread.py
#KOStore.py: KSpread.py
#KOffice.py: KSpread.py
#OpenParts.py: KSpread.py
#OpenPartsUI.py: KSpread.py
#KSpread.py: kspread.idl $(kde_bindir)/pyidl $(top_srcdir)/lib/kofficecore/koffice.idl
#	$(kde_bindir)/pyidl "-I$(idldir) -I$(top_srcdir)/lib/kofficecore -I$(top_srcdir)/lib/store -I$(interfacedir) $(srcdir)/kspread.idl"

