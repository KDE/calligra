KDE_CXXFLAGS = $(USE_RTTI)
INCLUDES  = $(KOFFICE_INCLUDES) $(KOTEXT_INCLUDES) -I$(interfacedir) \
	$(KOSCRIPT_INCLUDES) -I$(top_srcdir)/interfaces \
	 -I$(top_srcdir)/kchart/kdchart $(all_includes)

kde_module_LTLIBRARIES = libkspreadpart.la

## The part
libkspreadpart_la_SOURCES = kspread_locale.cc \
	kspread_dlg_formula.cc digest.cc \
	kspread_functions.cc kspread_dlg_csv.cc \
	kspread_canvas.cc kspread_functions_helper.cc \
	kspread_doc.cc kspread_numformat.cc \
	kspread_value.cc kspread_style.cc kspread_style_manager.cc \
	kspread_undo.cc kspread_view.cc kspread_tabbar.cc \
	kspread_autofill.cc kspread_interpreter.cc \
	kspread_functions_math.cc kspread_functions_conversion.cc \
	kspread_functions_information.cc  kspread_functions_engineering.cc \
	kspread_functions_logic.cc kspread_functions_financial.cc \
	kspread_functions_trig.cc kspread_functions_datetime.cc \
	kspread_functions_statistical.cc kspread_functions_text.cc kspread_functions_reference.cc \
	kspread_map.cc kspread_changes.cc kspread_dlg_layout.cc \
	kspread_dlg_scripts.cc kspread_cell.cc kspread_sheet.cc \
	kspread_dlg_cons.cc kspread_util.cc kspread_functions_database.cc \
	kspread_dlg_database.cc kspread_dlg_goalseek.cc kspread_dlg_styles.cc \
	kspread_editors.cc kspreadsubtotal.ui kspread_dlg_changes.cc \
	kspread_dlg_reference.cc kspread_dlg_subtotal.cc \
	kspread_dlg_comment.cc kspread_dlg_angle.cc kspread_dlg_goto.cc\
	kspread_dlg_area.cc kspread_dlg_resize2.cc kspread_dlg_preference.cc\
	kspread_dlg_special.cc kspread_scripting.cc \
	kspread_dlg_sort.cc kspread_dlg_anchor.cc kspread_dlg_conditional.cc\
	kspread_factory.cc kspread_handler.cc \
	kspread_events.cc kspread_dlg_show.cc kspread_dlg_series.cc \
	kspread_dlg_validity.cc kspread_dlg_showColRow.cc \
	kspread_dlg_list.cc \
	kspread_dlg_insert.cc kspread_dlg_pasteinsert.cc KSpreadDocIface.skel KSpreadMapIface.skel \
	KSpreadTableIface.skel KSpreadDocIface.cc KSpreadMapIface.cc \
	KSpreadTableIface.cc KSpreadCellIface.cc KSpreadCellIface.skel \
	KSpreadAppIface.cc KSpreadAppIface.skel kspread_dlg_format.cc \
	KSpreadViewIface.skel KSpreadViewIface.cc \
	kspread_cluster.cc kspread_depend.cc kspread_condition.cc \
	KSpreadLayoutIface.skel KSpreadLayoutIface.cc \
        KSpreadColumnIface.skel KSpreadColumnIface.cc \
	KSpreadRowIface.skel KSpreadRowIface.cc \
	kspread_selection.cc kspread_dlg_paperlayout.cc kspread_format.cc \
        kspread_sheetprint.cc

libkspreadpart_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN)
libkspreadpart_la_LIBADD = $(top_builddir)/interfaces/libkochart.la \
	$(LIB_KOSCRIPT) $(LIB_KOFFICEUI) $(LIB_KOTEXT) -lkspell

METASOURCES = AUTO

## The kdeinit loadable module
lib_LTLIBRARIES =  kspread.la
kspread_la_SOURCES = main.cc
kspread_la_LDFLAGS = $(all_libraries) -avoid-version -module
kspread_la_LIBADD = $(LIB_KOFFICECORE)

## The executable
bin_PROGRAMS = kspread
kspread_SOURCES = dummy.cc
kspread_LDFLAGS = $(all_libraries) $(KDE_RPATH)
kspread_LDADD = kspread.la

dummy.cc:
	echo > dummy.cc

noinst_HEADERS = kspread_doc.h kspread_functions.h kspread_functions_helper.h \
	kspread_undo.h kspread_view.h kspread_tabbar.h \
	kspread_autofill.h kspread_interpreter.h kspread_style.h kspread_style_manager.h \
	kspread_map.h kspread_dlg_layout.h \
	kspread_dlg_scripts.h kspread_sheet.h kspread_cell.h \
	kspread_dlg_cons.h kspread_cluster.h kspread_value.h \
	kspread_util.h kspread_canvas.h kspread_dlg_subtotal.h \
	kspread_dlg_reference.h kspread_locale.h kspread_changes.h \
	kspread_editors.h  kspread_dlg_formula.h kspread_dlg_changes.h \
	kspread_dlg_area.h kspread_dlg_resize2.h\
	kspread_dlg_special.h kspread_dlg_goto.h kspread_dlg_styles.h \
	kspread_dlg_sort.h kspread_dlg_anchor.h kspread_dlg_series.h \
	kspread_dlg_preference.h kspread_dlg_angle.h \
	kspread_factory.h kspread_handler.h \
	kspread_dlg_validity.h  kspread_dlg_showColRow.h \
	kspread_dlg_list.h kspread_dlg_csv.h \
	kspread_dlg_database.h kspread_dlg_goalseek.h \
	kspread_events.h kspread_global.h \
	kspread_dlg_show.h kspread_dlg_insert.h kspread_dlg_pasteinsert.h KSpreadViewIface.h \
	KSpreadCellIface.h KSpreadMapIface.h KSpreadTableIface.h KSpreadDocIface.h \
	KSpreadAppIface.h kspread_dlg_format.h kspread_aboutdata.h \
	kspread_depend.h kspread_condition.h KSpreadLayoutIface.h \
	KSpreadColumnIface.h KSpreadRowIface.h kspread_selection.h \
	kspread_dlg_paperlayout.h kspread_format.h \
        kspread_sheetprint.h

kdelnk_DATA = kspread.desktop
kdelnkdir = $(kde_appsdir)/Office

rcdir = $(kde_datadir)/kspread
rc_DATA = kspread.rc kspread_readonly.rc

SUBDIRS =  . plugins pics dtd toolbar extensions tablestyles templates

parser:
	flex lex.l
	yacc -d yacc.y
	flex -Pdep lex.dep.l
	bison -p dep -b dep -d yacc.dep.y

messages: rc.cpp
	$(EXTRCACRC) --tag-group=koffice extensions/*.xml > xml_doc.cc
	$(EXTRACTRC) *.ui >> rc.cpp
	$(XGETTEXT) rc.cpp *.cc kspread_aboutdata.h -o $(podir)/kspread.pot
	-rm xml_doc.cc

kde_services_DATA = kspreadpart.desktop
