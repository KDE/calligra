INCLUDES  = $(KOFFICE_INCLUDES) -I$(interfacedir) $(all_includes) \
-I$(top_srcdir)/lib/kscript -I$(top_builddir)/lib/kscript

kspread_LDFLAGS = $(all_libraries) $(KDE_RPATH)
kspread_LDADD = ../kchart/libkchart.la libkspread.la ../lib/kofficeui/libkofficeui.la

libkspread_la_LDFLAGS = -avoid-version -module

libkspread_la_LIBADD = $(all_libraries) ../lib/kscript/libkscript.la ../lib/kofficeui/libkofficeui.la -lkformula

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
	kspread_dlg_formula.cc kspread_dlg_create.cc \
	kspread_dlg_special.cc kspread_dlg_goto.cc  kspread_dlg_replace.cc \
	kspread_dlg_sort.cc kspread_dlg_anchor.cc \
	kspread_dlg_resize.cc kspread_factory.cc kspread_handler.cc \
	kspread_events.cc  kspread_dlg_assistant2.cc kspread_dlg_show.cc \
	kspread_dlg_insert.cc KSpreadDocIface_skel.cpp KSpreadMapIface_skel.cpp \
	KSpreadTableIface_skel.cpp KSpreadDocIface.cc KSpreadMapIface.cc \
	KSpreadTableIface.cc KSpreadCellIface.cc KSpreadCellIface_skel.cpp

kspread_SOURCES = main.cc

noinst_HEADERS = kspread_shell.h kspread_doc.h \
	kspread_undo.h kspread_view.h kspread_tabbar.h \
	kspread_autofill.h kspread_interpreter.h \
	kspread_layout.h kspread_map.h kspread_dlg_layout.h \
	kspread_dlg_scripts.h kspread_table.h kspread_cell.h \
	kspread_dlg_cons.h \
	kspread_util.h kspread_canvas.h \
	kspread_editors.h kspread_dlg_tabname.h kspread_dlg_formula.h \
	kspread_dlg_special.h kspread_dlg_goto.h kspread_dlg_replace.h \
	kspread_dlg_sort.h kspread_dlg_create.h kspread_dlg_anchor.h  \
	kspread_dlg_resize.h kspread_factory.h kspread_handler.h \
	kspread_events.h kspread_global.h  \
	kspread_dlg_assistant2.h kspread_dlg_show.h kspread_dlg_insert.h \
	KSpreadCellIface.h KSpreadMapIface.h KSpreadTableIface.h KSpreadDocIface.h

libkspread_la_METASOURCES = kspread_shell.moc kspread_doc.moc \
		  	    kspread_view.moc kspread_tabbar.moc kspread_canvas.moc \
			    kspread_table.moc kspread_dlg_layout.moc kspread_cell.moc \
			    kspread_dlg_scripts.moc kspread_part.moc kspread_dlg_cons.moc \
			    kspread_editors.moc kspread_dlg_tabname.moc kspread_dlg_formula.moc \
			    kspread_dlg_special.moc kspread_dlg_goto.moc kspread_dlg_replace.moc \
			    kspread_dlg_sort.moc  kspread_dlg_create.moc \
			    kspread_dlg_anchor.moc kspread_dlg_assistant2.moc kspread_dlg_show.moc kspread_dlg_insert.moc \
			    kspread_dlg_resize.moc kspread_factory.moc kspread_handler.moc

kdelnk_DATA = kspread.desktop
kdelnkdir = $(kde_appsdir)/KOffice

kdemime_DATA = x-kspread.desktop
kdemimedir = $(kde_mimedir)/application

rcdir = $(kde_datadir)/kspread
rc_DATA = kspread.rc kspread_shell.rc

SUBDIRS =  . plugins pics dtd toolbar

KSpreadDocIface.kidl: KSpreadDocIface.h
	( dcopidl KSpreadDocIface.h >KSpreadDocIface.kidl; dcopidl2cpp --no-stub KSpreadDocIface.kidl )
KSpreadDocIface_skel.cpp: KSpreadDocIface.kidl
KSpreadTableIface.kidl: KSpreadTableIface.h
	( dcopidl KSpreadTableIface.h >KSpreadTableIface.kidl; dcopidl2cpp --no-stub KSpreadTableIface.kidl )
KSpreadTableIface_skel.cpp: KSpreadTableIface.kidl
KSpreadMapIface.kidl: KSpreadMapIface.h
	( dcopidl KSpreadMapIface.h >KSpreadMapIface.kidl; dcopidl2cpp --no-stub KSpreadMapIface.kidl )
KSpreadMapIface_skel.cpp: KSpreadMapIface.kidl
KSpreadCellIface.kidl: KSpreadCellIface.h
	( dcopidl KSpreadCellIface.h >KSpreadCellIface.kidl; dcopidl2cpp --no-stub KSpreadCellIface.kidl )
KSpreadCellIface_skel.cpp: KSpreadCellIface.kidl

parser:
	flex lex.l
	yacc -d yacc.y
	flex -Pdep lex.dep.l
	bison -p dep -b dep -d yacc.dep.y

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot *.cc && mv messages.po ../po/kspread.pot


