

include_directories( ${CMAKE_SOURCE_DIR}/karbon ${CMAKE_SOURCE_DIR}/karbon/core ${KDE4_INCLUDES}  )


########### next target ###############

set(karbon_flattenpathplugin_PART_SRCS flattenpathplugin.cc )

kde4_automoc(${karbon_flattenpathplugin_PART_SRCS})

kde4_add_plugin(karbon_flattenpathplugin ${karbon_flattenpathplugin_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} karbon_flattenpathplugin )

target_link_libraries(karbon_flattenpathplugin  ${KDE4_KDECORE_LIBS} ${KDE4_KPARTS_LIBS} kofficeui karboncommon )

install(TARGETS karbon_flattenpathplugin  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  flattenpathplugin.rc  DESTINATION ${DATA_INSTALL_DIR}/karbon/kpartplugins)





#original Makefile.am contents follow:

#INCLUDES = $(KOFFICE_INCLUDES) -I$(top_srcdir)/karbon  -I$(top_srcdir)/karbon/core $(all_includes)
#
#kde_module_LTLIBRARIES = karbon_flattenpathplugin.la
#
#karbon_flattenpathplugin_la_SOURCES = flattenpathplugin.cc
#karbon_flattenpathplugin_la_LIBADD = $(LIB_KPARTS) $(LIB_KOFFICEUI) \
#				../../libkarboncommon.la
#
#karbon_flattenpathplugin_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
#
#partpluginsdir   = $(kde_datadir)/karbon/kpartplugins
#partplugins_DATA = flattenpathplugin.rc
#
#METASOURCES = AUTO
#
