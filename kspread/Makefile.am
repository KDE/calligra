KDE_CXXFLAGS = $(USE_RTTI)
INCLUDES  = $(KOFFICE_INCLUDES) $(KOTEXT_INCLUDES) -I$(interfacedir) \
	-I$(srcdir)/tests \
	-Idialogs \
	-I$(top_srcdir)/interfaces \
	 -I$(top_srcdir)/kchart/kdchart $(all_includes)

lib_LTLIBRARIES = libkspreadcommon.la
kde_module_LTLIBRARIES = libkspreadpart.la

libkspreadcommon_la_SOURCES = digest.cc commands.cc damages.cc dependencies.cc \
	formula.cc 	functions.cc kspread_autofill.cc kspread_canvas.cc \
	kspread_cell.cc kspread_cluster.cc kspread_condition.cc kspread_doc.cc \
	kspread_editors.cc kspread_events.cc kspread_factory.cc kspread_format.cc \
	kspread_functions_conversion.cc kspread_functions_database.cc kspread_functions_datetime.cc \
	kspread_functions_engineering.cc kspread_functions_financial.cc kspread_functions_helper.cc \
	kspread_functions_information.cc kspread_functions_logic.cc kspread_functions_math.cc \
	kspread_functions_reference.cc kspread_functions_statistical.cc kspread_functions_text.cc \
	kspread_functions_trig.cc kspread_handler.cc kspread_locale.cc kspread_map.cc \
	kspread_numformat.cc kspread_object.cc kspread_style.cc kspread_style_manager.cc \
	kspread_undo.cc kspread_value.cc kspread_view.cc kspread_sheet.cc kspread_sheetprint.cc \
	kspread_util.cc kspread_toolbox.cc 	valuecalc.cc 	valueconverter.cc 	valueformatter.cc \
		valueparser.cc KSpreadAppIface.cc KSpreadAppIface.skel KSpreadCellIface.cc \
	KSpreadCellIface.skel KSpreadColumnIface.skel KSpreadColumnIface.cc KSpreadDocIface.cc \
	KSpreadDocIface.skel KSpreadLayoutIface.skel KSpreadLayoutIface.cc KSpreadMapIface.cc \
	KSpreadMapIface.skel KSpreadRowIface.skel KSpreadRowIface.cc KSpreadTableIface.cc \
	KSpreadTableIface.skel KSpreadViewIface.skel KSpreadViewIface.cc \
		kspread_genvalidationstyle.cc region.cc selection.cc manipulator.cc manipulator_data.cc \
	kspread_generalProperty.cpp kspread_propertyEditor.cpp 

libkspreadcommon_la_LDFLAGS = $(all_libraries) $(KDE_RPATH) -no-undefined
libkspreadcommon_la_LIBADD = $(top_builddir)/interfaces/libkochart.la \
	dialogs/libdialogs.la tests/libtests.la \
	$(LIB_KOFFICEUI) $(LIB_KOTEXT) -lkspell

libkspreadpart_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN)
libkspreadpart_la_LIBADD  = libkspreadcommon.la
libkspreadpart_la_SOURCES = kspread_factory_init.cc


METASOURCES = AUTO

## The kdeinit loadable module and the executable
kdeinit_LTLIBRARIES = kspread.la
bin_PROGRAMS =
kspread_la_SOURCES = main.cc
kspread_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
kspread_la_LIBADD = $(LIB_KOFFICECORE)

kdelnk_DATA = kspread.desktop
kdelnkdir = $(kde_appsdir)/Office

kde_services_DATA = kspreadpart.desktop

rcdir = $(kde_datadir)/kspread
rc_DATA = kspread.rc kspread_readonly.rc

SUBDIRS = dialogs tests . plugins pics dtd toolbar extensions sheetstyles templates 

parser:
	flex lex.l
	yacc -d yacc.y
	flex -Pdep lex.dep.l
	bison -p dep -b dep -d yacc.dep.y

messages: rc.cpp
	$(EXTRACTRC) --tag-group=koffice extensions/*.xml > xml_doc.cc
	$(EXTRACTRC) dialogs/*.ui >> rc.cpp
	$(XGETTEXT) rc.cpp *.cc kspread_aboutdata.h dialogs/*.cc tests/*.cc -o $(podir)/kspread.pot
	-rm xml_doc.cc

include ../admin/Doxyfile.am

noinst_HEADERS =
