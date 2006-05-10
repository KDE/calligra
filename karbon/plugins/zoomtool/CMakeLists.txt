kde4_header()

include_directories( ${CMAKE_SOURCE_DIR}/karbon ${CMAKE_SOURCE_DIR}/karbon/core ${KDE4_INCLUDES}  )


########### next target ###############

set(karbon_zoomtoolplugin_PART_SRCS vzoomtool.cc zoomtoolplugin.cc )

kde4_automoc(${karbon_zoomtoolplugin_PART_SRCS})

kde4_add_plugin(karbon_zoomtoolplugin ${karbon_zoomtoolplugin_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} karbon_zoomtoolplugin )

target_link_libraries(karbon_zoomtoolplugin  ${KDE4_KDECORE_LIBS} ${KDE4_KPARTS_LIBS} kofficeui karboncommon )

install_targets(${PLUGIN_INSTALL_DIR} karbon_zoomtoolplugin )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES  karbonzoomtool.desktop )

kde4_footer()



#original Makefile.am contents follow:

#kde_services_DATA = karbonzoomtool.desktop
#
#INCLUDES = $(KOFFICE_INCLUDES) -I$(top_srcdir)/karbon  -I$(top_srcdir)/karbon/core $(all_includes)
#
#kde_module_LTLIBRARIES = karbon_zoomtoolplugin.la
#
#karbon_zoomtoolplugin_la_SOURCES = vzoomtool.cc zoomtoolplugin.cc
#karbon_zoomtoolplugin_la_LIBADD = $(LIB_KPARTS) $(LIB_KOFFICEUI) \
#				../../libkarboncommon.la
#
#karbon_zoomtoolplugin_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
#
#METASOURCES = AUTO
#
