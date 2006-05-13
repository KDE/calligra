

include_directories( 
	${KOTEXT_INCLUDES}
	${CMAKE_SOURCE_DIR}/kspread 
	${KDE4_INCLUDE_DIR} 
	${QT_INCLUDES}  )


########### next target ###############

set(csvimport_PART_SRCS csvimport.cc xmltree.cc csvdialog.cpp )

kde4_automoc(${csvimport_PART_SRCS})

kde4_add_ui3_files(csvimport_PART_SRCS dialogui.ui )

kde4_add_plugin(csvimport WITH_PREFIX ${csvimport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} csvimport )

target_link_libraries(csvimport  kspreadcommon )

install_targets(${PLUGIN_INSTALL_DIR} csvimport )


########### next target ###############

set(csvexport_PART_SRCS csvexport.cc csvexportdialog.cpp )

kde4_automoc(${csvexport_PART_SRCS})

kde4_add_ui3_files(csvexport_PART_SRCS exportdialogui.ui )

kde4_add_plugin(csvexport WITH_PREFIX ${csvexport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} csvexport )

target_link_libraries(csvexport  kspreadcommon )

install_targets(${PLUGIN_INSTALL_DIR} csvexport )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES  kspread_csv_import.desktop kspread_csv_export.desktop )





#original Makefile.am contents follow:

######## General stuff
#
#INCLUDES= -I$(srcdir) -I$(top_srcdir)/kspread $(KOFFICE_INCLUDES) \
#	$(KOTEXT_INCLUDES)  $(all_includes)
#
######## Files
#
#kde_module_LTLIBRARIES = libcsvimport.la libcsvexport.la
#
#libcsvimport_la_SOURCES = csvimport.cc xmltree.cc csvdialog.cpp dialogui.ui
#libcsvimport_la_LDFLAGS	= $(all_libraries) -module -avoid-version -no-undefined 
#libcsvimport_la_LIBADD  = $(KOFFICE_LIBS) ../../../kspread/libkspreadcommon.la
#
#libcsvexport_la_SOURCES = csvexport.cc csvexportdialog.cpp exportdialogui.ui
#libcsvexport_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
#libcsvexport_la_LIBADD  = $(KOFFICE_LIBS) ../../../kspread/libkspreadcommon.la
#
#METASOURCES = AUTO
#
#service_DATA = kspread_csv_import.desktop kspread_csv_export.desktop
#
#servicedir = $(kde_servicesdir)
