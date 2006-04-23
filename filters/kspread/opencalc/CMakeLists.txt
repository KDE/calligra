kde4_header()

include_directories( ${CMAKE_SOURCE_DIR}/kspread ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(opencalcimport_PART_SRCS opencalcimport.cc ${liboofilter_SRCS} )

kde4_automoc(${opencalcimport_PART_SRCS})

kde4_add_plugin(opencalcimport WITH_PREFIX ${opencalcimport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} opencalcimport )

target_link_libraries(opencalcimport  ${KDE4_KDECORE_LIBS} kspreadcommon  )

install_targets(${PLUGIN_INSTALL_DIR} opencalcimport )


########### next target ###############

set(opencalcexport_PART_SRCS opencalcexport.cc opencalcstyleexport.cc ${liboofilter_SRCS})

kde4_automoc(${opencalcexport_PART_SRCS})

kde4_add_plugin(opencalcexport WITH_PREFIX ${opencalcexport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} opencalcexport )

target_link_libraries(opencalcexport  ${KDE4_KDECORE_LIBS} kspreadcommon  )

install_targets(${PLUGIN_INSTALL_DIR} opencalcexport )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES  kspread_opencalc_import.desktop kspread_opencalc_export.desktop )

kde4_footer()



#original Makefile.am contents follow:

######## General stuff
#
#INCLUDES= -I$(srcdir)/../../liboofilter -I$(srcdir) -I$(top_srcdir)/kspread $(KOFFICE_INCLUDES) \
#	$(KOTEXT_INCLUDES) $(all_includes)
#
######## Files
#kde_module_LTLIBRARIES = libopencalcimport.la libopencalcexport.la
#
#libopencalcexport_la_SOURCES = opencalcexport.cc opencalcstyleexport.cc
#libopencalcexport_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
#libopencalcexport_la_LIBADD  = ../../../kspread/libkspreadcommon.la ../../liboofilter/liboofilter.la $(KOFFICE_LIBS)
#
#libopencalcimport_la_SOURCES = opencalcimport.cc
#libopencalcimport_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
#libopencalcimport_la_LIBADD  = ../../../kspread/libkspreadcommon.la ../../liboofilter/liboofilter.la $(KOFFICE_LIBS)
#
#METASOURCES = AUTO
#
#service_DATA = kspread_opencalc_import.desktop kspread_opencalc_export.desktop
#
#servicedir = $(kde_servicesdir)
