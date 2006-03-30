kde4_header()
project(kspread)

add_subdirectory( dialogs )
add_subdirectory( tests )
add_subdirectory( plugins )
add_subdirectory( pics )
add_subdirectory( dtd )
add_subdirectory( toolbar )
add_subdirectory( extensions )
add_subdirectory( sheetstyles )
add_subdirectory( templates )

include_directories( ${CMAKE_SOURCE_DIR}/interfaces ${CMAKE_SOURCE_DIR}/kchart/kdchart ${KOTEXT_INCLUDES} ${KOFFICEUI_INCLUDES} ${KOFFICECORE_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############
set(libdialogs_SRCS
	${CMAKE_SOURCE_DIR}/kspread/dialogs/kspread_dlg_angle.cc
	${CMAKE_SOURCE_DIR}/kspread/dialogs/kspread_dlg_area.cc
	${CMAKE_SOURCE_DIR}/kspread/dialogs/kspread_dlg_comment.cc
	${CMAKE_SOURCE_DIR}/kspread/dialogs/kspread_dlg_conditional.cc
	${CMAKE_SOURCE_DIR}/kspread/dialogs/kspread_dlg_cons.cc
	${CMAKE_SOURCE_DIR}/kspread/dialogs/kspread_dlg_csv.cc
	${CMAKE_SOURCE_DIR}/kspread/dialogs/kspread_dlg_database.cc
	${CMAKE_SOURCE_DIR}/kspread/dialogs/kspread_dlg_format.cc
	${CMAKE_SOURCE_DIR}/kspread/dialogs/kspread_dlg_formula.cc
	${CMAKE_SOURCE_DIR}/kspread/dialogs/kspread_dlg_goalseek.cc
	${CMAKE_SOURCE_DIR}/kspread/dialogs/kspread_dlg_goto.cc
	${CMAKE_SOURCE_DIR}/kspread/dialogs/kspread_dlg_insert.cc
	${CMAKE_SOURCE_DIR}/kspread/dialogs/kspread_dlg_layout.cc
	${CMAKE_SOURCE_DIR}/kspread/dialogs/kspread_dlg_list.cc
	${CMAKE_SOURCE_DIR}/kspread/dialogs/kspread_dlg_paperlayout.cc
	${CMAKE_SOURCE_DIR}/kspread/dialogs/kspread_dlg_pasteinsert.cc
	${CMAKE_SOURCE_DIR}/kspread/dialogs/kspread_dlg_preference.cc
	${CMAKE_SOURCE_DIR}/kspread/dialogs/kspread_dlg_reference.cc
	${CMAKE_SOURCE_DIR}/kspread/dialogs/kspread_dlg_resize2.cc
	${CMAKE_SOURCE_DIR}/kspread/dialogs/kspread_dlg_series.cc
	${CMAKE_SOURCE_DIR}/kspread/dialogs/kspread_dlg_show.cc
	${CMAKE_SOURCE_DIR}/kspread/dialogs/kspread_dlg_showColRow.cc
	${CMAKE_SOURCE_DIR}/kspread/dialogs/kspread_dlg_sort.cc
	${CMAKE_SOURCE_DIR}/kspread/dialogs/kspread_dlg_special.cc
	${CMAKE_SOURCE_DIR}/kspread/dialogs/kspread_dlg_styles.cc
	${CMAKE_SOURCE_DIR}/kspread/dialogs/kspread_dlg_subtotal.cc
	${CMAKE_SOURCE_DIR}/kspread/dialogs/kspread_dlg_validity.cc
	${CMAKE_SOURCE_DIR}/kspread/dialogs/link.cc
	${CMAKE_SOURCE_DIR}/kspread/dialogs/sheet_properties.cc
	${CMAKE_SOURCE_DIR}/kspread/dialogs/kspread_dlg_find.cc
)


kde4_add_ui3_files(libdialogs_SRCS
	${CMAKE_SOURCE_DIR}/kspread/dialogs/kspreadsubtotal.ui
	${CMAKE_SOURCE_DIR}/kspread/dialogs/sheet_properties_base.ui
	${CMAKE_SOURCE_DIR}/kspread/dialogs/position_cell_format.ui
	${CMAKE_SOURCE_DIR}/kspread/dialogs/font_cell_format.ui
	${CMAKE_SOURCE_DIR}/kspread/dialogs/protection_cell_format.ui
	${CMAKE_SOURCE_DIR}/kspread/dialogs/SheetSelectWidget.ui
	)

set(kspreadcommon_LIB_SRCS ${libdialogs_SRCS}
   digest.cc
   commands.cc
   damages.cc
   dependencies.cc
   formula.cc
   functions.cc
   kspread_autofill.cc
   kspread_canvas.cc
   kspread_cell.cc
   kspread_cluster.cc
   kspread_condition.cc
   kspread_doc.cc
   kspread_editors.cc
   kspread_events.cc
   kspread_factory.cc
   kspread_format.cc
   kspread_functions_conversion.cc
   kspread_functions_database.cc
   kspread_functions_datetime.cc
   kspread_functions_engineering.cc
   kspread_functions_financial.cc
   kspread_functions_helper.cc
   kspread_functions_information.cc
   kspread_functions_logic.cc
   kspread_functions_math.cc
   kspread_functions_reference.cc
   kspread_functions_statistical.cc
   kspread_functions_text.cc
   kspread_functions_trig.cc
   kspread_handler.cc
   kspread_locale.cc
   kspread_map.cc
   kspread_numformat.cc
   kspread_object.cc
   kspread_style.cc
   kspread_style_manager.cc
   kspread_undo.cc
   kspread_value.cc
   kspread_view.cc
   kspread_sheet.cc
   kspread_sheetprint.cc
   kspread_util.cc
   kspread_toolbox.cc
   valuecalc.cc
   valueconverter.cc
   valueformatter.cc
   valueparser.cc
   KSpreadAppIface.cc
   KSpreadCellIface.cc
   KSpreadColumnIface.cc
   KSpreadDocIface.cc
   KSpreadLayoutIface.cc
   KSpreadMapIface.cc
   KSpreadRowIface.cc
   KSpreadTableIface.cc
   KSpreadViewIface.cc
   kspread_genvalidationstyle.cc
   region.cc
   selection.cc
   manipulator.cc
   manipulator_data.cc
   kspread_generalProperty.cpp
   kspread_propertyEditor.cpp )

kde4_automoc(${kspreadcommon_LIB_SRCS})

kde4_add_dcop_skels(kspreadcommon_LIB_SRCS
   KSpreadAppIface.h
   KSpreadCellIface.h
   KSpreadColumnIface.h
   KSpreadDocIface.h
   KSpreadLayoutIface.h
   KSpreadMapIface.h
   KSpreadRowIface.h
   KSpreadTableIface.h
   KSpreadViewIface.h )

kde4_add_library(kspreadcommon SHARED ${kspreadcommon_LIB_SRCS})

target_link_libraries(kspreadcommon  ${KDE4_KDECORE_LIBS} kochart dialogs tests kspell )

set_target_properties(kspreadcommon PROPERTIES VERSION 1.0.0 SOVERSION 1 )
install_targets(${LIB_INSTALL_DIR} kspreadcommon )


########### next target ###############

set(kspreadpart_PART_SRCS kspread_factory_init.cc )

kde4_automoc(${kspreadpart_PART_SRCS})

kde4_add_plugin(kspreadpart WITH_PREFIX ${kspreadpart_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kspreadpart )

target_link_libraries(kspreadpart  ${KDE4_KDECORE_LIBS} kspreadcommon )

install_targets(${LIB_INSTALL_DIR}/kde4 kspreadpart )


########### next target ###############

set(kspread_KDEINIT_SRCS main.cc )

kde4_automoc(${kspread_KDEINIT_SRCS})

kde4_add_kdeinit_executable( kspread ${kspread_KDEINIT_SRCS})

target_link_libraries(kdeinit_kspread  ${KDE4_KDECORE_LIBS} kofficecore )

install_targets(${LIB_INSTALL_DIR} kdeinit_kspread )

target_link_libraries( kspread kdeinit_kspread )
install_targets(/bin kspread )

########### install files ###############

install_files( ${DATA_INSTALL_DIR}/kspread FILES  kspread.rc kspread_readonly.rc )
install_files( ${SERVICES_INSTALL_DIR} FILES  kspreadpart.desktop )
install_files( ${APPLNK_INSTALL_DIR}/Office FILES  kspread.desktop )

kde4_create_doxygen_docs(  )

kde4_footer()



#original Makefile.am contents follow:

#KDE_CXXFLAGS = $(USE_RTTI)
#INCLUDES  = $(KOFFICE_INCLUDES) $(KOTEXT_INCLUDES) -I$(interfacedir) \
#	-I$(srcdir)/tests \
#	-Idialogs \
#	-I$(top_srcdir)/interfaces \
#	 -I$(top_srcdir)/kchart/kdchart $(all_includes)
#
#lib_LTLIBRARIES = libkspreadcommon.la
#kde_module_LTLIBRARIES = libkspreadpart.la
#
#libkspreadcommon_la_SOURCES = digest.cc commands.cc damages.cc dependencies.cc \
#	formula.cc 	functions.cc kspread_autofill.cc kspread_canvas.cc \
#	kspread_cell.cc kspread_cluster.cc kspread_condition.cc kspread_doc.cc \
#	kspread_editors.cc kspread_events.cc kspread_factory.cc kspread_format.cc \
#	kspread_functions_conversion.cc kspread_functions_database.cc kspread_functions_datetime.cc \
#	kspread_functions_engineering.cc kspread_functions_financial.cc kspread_functions_helper.cc \
#	kspread_functions_information.cc kspread_functions_logic.cc kspread_functions_math.cc \
#	kspread_functions_reference.cc kspread_functions_statistical.cc kspread_functions_text.cc \
#	kspread_functions_trig.cc kspread_handler.cc kspread_locale.cc kspread_map.cc \
#	kspread_numformat.cc kspread_object.cc kspread_style.cc kspread_style_manager.cc \
#	kspread_undo.cc kspread_value.cc kspread_view.cc kspread_sheet.cc kspread_sheetprint.cc \
#	kspread_util.cc kspread_toolbox.cc 	valuecalc.cc 	valueconverter.cc 	valueformatter.cc \
#		valueparser.cc KSpreadAppIface.cc KSpreadAppIface.skel KSpreadCellIface.cc \
#	KSpreadCellIface.skel KSpreadColumnIface.skel KSpreadColumnIface.cc KSpreadDocIface.cc \
#	KSpreadDocIface.skel KSpreadLayoutIface.skel KSpreadLayoutIface.cc KSpreadMapIface.cc \
#	KSpreadMapIface.skel KSpreadRowIface.skel KSpreadRowIface.cc KSpreadTableIface.cc \
#	KSpreadTableIface.skel KSpreadViewIface.skel KSpreadViewIface.cc \
#		kspread_genvalidationstyle.cc region.cc selection.cc manipulator.cc manipulator_data.cc \
#	kspread_generalProperty.cpp kspread_propertyEditor.cpp
#
#libkspreadcommon_la_LDFLAGS = $(all_libraries) $(KDE_RPATH) -no-undefined
#libkspreadcommon_la_LIBADD = $(top_builddir)/interfaces/libkochart.la \
#	dialogs/libdialogs.la tests/libtests.la \
#	$(LIB_KOFFICEUI) $(LIB_KOTEXT) -lkspell
#
#libkspreadpart_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN)
#libkspreadpart_la_LIBADD  = libkspreadcommon.la
#libkspreadpart_la_SOURCES = kspread_factory_init.cc
#
#
#METASOURCES = AUTO
#
### The kdeinit loadable module and the executable
#kdeinit_LTLIBRARIES = kspread.la
#bin_PROGRAMS =
#kspread_la_SOURCES = main.cc
#kspread_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
#kspread_la_LIBADD = $(LIB_KOFFICECORE)
#
#kdelnk_DATA = kspread.desktop
#kdelnkdir = $(kde_appsdir)/Office
#
#kde_services_DATA = kspreadpart.desktop
#
#rcdir = $(kde_datadir)/kspread
#rc_DATA = kspread.rc kspread_readonly.rc
#
#SUBDIRS = dialogs tests . plugins pics dtd toolbar extensions sheetstyles templates
#
#parser:
#	flex lex.l
#	yacc -d yacc.y
#	flex -Pdep lex.dep.l
#	bison -p dep -b dep -d yacc.dep.y
#
#messages: rc.cpp
#	$(EXTRACTRC) --tag-group=koffice extensions/*.xml > xml_doc.cc
#	$(EXTRACTRC) dialogs/*.ui >> rc.cpp
#	$(XGETTEXT) rc.cpp *.cc kspread_aboutdata.h dialogs/*.cc tests/*.cc -o $(podir)/kspread.pot
#	-rm xml_doc.cc
#
#include ../admin/Doxyfile.am
#
#noinst_HEADERS =
