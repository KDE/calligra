

include_directories( ${CMAKE_SOURCE_DIR}/karbon ${CMAKE_SOURCE_DIR}/karbon/core ${KDE4_INCLUDES}  )


########### next target ###############

set(karbon_imagetoolplugin_PART_SRCS imagetoolplugin.cc vimagetool.cc )

kde4_automoc(${karbon_imagetoolplugin_PART_SRCS})

kde4_add_plugin(karbon_imagetoolplugin ${karbon_imagetoolplugin_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} karbon_imagetoolplugin )

target_link_libraries(karbon_imagetoolplugin  ${KDE4_KDECORE_LIBS} ${KDE4_KPARTS_LIBS} kofficeui karboncommon )

install_targets(${PLUGIN_INSTALL_DIR} karbon_imagetoolplugin )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES  karbonimagetool.desktop )





#original Makefile.am contents follow:

#kde_services_DATA = karbonimagetool.desktop
#
#INCLUDES = $(KOFFICE_INCLUDES) -I$(top_srcdir)/karbon  -I$(top_srcdir)/karbon/core $(all_includes)
#
#kde_module_LTLIBRARIES = karbon_imagetoolplugin.la
#
#karbon_imagetoolplugin_la_SOURCES = imagetoolplugin.cc vimagetool.cc
#karbon_imagetoolplugin_la_LIBADD = $(LIB_KPARTS) $(LIB_KOFFICEUI) \
#				../../libkarboncommon.la
#
#karbon_imagetoolplugin_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
#
#METASOURCES = AUTO
#
