INCLUDES  = $(KOFFICE_INCLUDES) -I$(interfacedir) $(all_includes) \
-I$(top_srcdir)/lib/kscript -I$(top_builddir)/lib/kscript

kspread_LDFLAGS = $(all_libraries) $(KDE_RPATH)
kspread_LDADD = libkspread.la

libkspread_la_LDFLAGS = -avoid-version -module -no-undefined

libkspread_la_LIBADD = $(all_libraries) ../kchart/libkchart.la ../lib/kscript/libkscript.la ../lib/kofficeui/libkofficeui.la -lkformula

####### Files

bin_PROGRAMS = kspread
lib_LTLIBRARIES = libkspread.la

libkspread_la_SOURCES = kspread_canvas.cc \
	kspread_shell.cc kspread_doc.cc \
	kspread_undo.cc kspread_view.cc kspread_tabbar.cc \
	kspread_autofill.cc kspread_interpreter.cc \
	kspread_layout.cc kspread_map.cc kspread_dlg_layout.cc \
	kspread_dlg_scripts.cc kspread_cell.cc kspread_table.cc \
	kspread_dlg_cons.cc kspread_util.cc \
	kspread_editors.cc kspread_dlg_tabname.cc \
	kspread_dlg_formula.cc kspread_dlg_reference.cc\
	kspread_dlg_area.cc kspread_dlg_resize2.cc kspread_dlg_preference.cc\
	kspread_dlg_special.cc kspread_dlg_goto.cc  kspread_dlg_replace.cc \
	kspread_dlg_sort.cc kspread_dlg_anchor.cc kspread_dlg_conditional.cc\
	kspread_dlg_resize.cc kspread_factory.cc kspread_handler.cc \
	kspread_events.cc kspread_dlg_show.cc kspread_dlg_series.cc \
	kspread_dlg_insert.cc KSpreadDocIface.skel KSpreadMapIface.skel \
	KSpreadTableIface.skel KSpreadDocIface.cc KSpreadMapIface.cc \
	KSpreadTableIface.cc KSpreadCellIface.cc KSpreadCellIface.skel \
	KSpreadAppIface.cc KSpreadAppIface.skel kspread_dlg_format.cc \
	kspread_dlg_oszi.cc KSpreadViewIface.skel KSpreadViewIface.cc \
	qtai_bar.cpp qtai_base.cpp qtai_meter.cpp qtai_plot.cpp qtai_strip.cpp

kspread_SOURCES = main.cc

noinst_HEADERS = kspread_shell.h kspread_doc.h \
	kspread_undo.h kspread_view.h kspread_tabbar.h \
	kspread_autofill.h kspread_interpreter.h \
	kspread_layout.h kspread_map.h kspread_dlg_layout.h \
	kspread_dlg_scripts.h kspread_table.h kspread_cell.h \
	kspread_dlg_cons.h \
	kspread_util.h kspread_canvas.h \
	kspread_editors.h kspread_dlg_tabname.h kspread_dlg_formula.h \
	kspread_dlg_reference.h kspread_dlg_area.h kspread_dlg_resize.h\
	kspread_dlg_special.h kspread_dlg_goto.h kspread_dlg_replace.h \
	kspread_dlg_sort.h kspread_dlg_anchor.h kspread_dlg_series.h \
	kspread_dlg_preference.h \
	kspread_dlg_resize.h kspread_factory.h kspread_handler.h \
	kspread_events.h kspread_global.h  \
	kspread_dlg_show.h kspread_dlg_insert.h KSpreadViewIface.h \
	KSpreadCellIface.h KSpreadMapIface.h KSpreadTableIface.h KSpreadDocIface.h \
	KSpreadAppIface.h kspread_dlg_format.h kspread_dlg_oszi.h \
	qtai_bar.h qtai_base.h qtai_meter.h qtai_plot.h qtai_strip.h

libkspread_la_METASOURCES = AUTO

kdelnk_DATA = kspread.desktop
kdelnkdir = $(kde_appsdir)/Applications/KOffice

kdemime_DATA = x-kspread.desktop
kdemimedir = $(kde_mimedir)/application

rcdir = $(kde_datadir)/kspread
rc_DATA = kspread.rc

SUBDIRS =  . plugins pics dtd toolbar scripts extensions tablestyles

parser:
	flex lex.l
	yacc -d yacc.y
	flex -Pdep lex.dep.l
	bison -p dep -b dep -d yacc.dep.y

messages: rc.cpp
	$(XGETTEXT) rc.cpp *.cc -o $(podir)/kspread.pot


