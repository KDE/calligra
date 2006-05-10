kde4_header()

include_directories( ${CMAKE_SOURCE_DIR}/karbon ${CMAKE_SOURCE_DIR}/karbon/core ${KDE4_INCLUDES}  )


########### next target ###############

set(karbon_roundcornersplugin_PART_SRCS roundcornersplugin.cc )

kde4_automoc(${karbon_roundcornersplugin_PART_SRCS})

kde4_add_plugin(karbon_roundcornersplugin ${karbon_roundcornersplugin_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} karbon_roundcornersplugin )

target_link_libraries(karbon_roundcornersplugin  ${KDE4_KDECORE_LIBS} ${KDE4_KPARTS_LIBS} karboncommon )

install_targets(${PLUGIN_INSTALL_DIR} karbon_roundcornersplugin )


########### install files ###############

install_files( ${DATA_INSTALL_DIR}/karbon/kpartplugins FILES  roundcornersplugin.rc )

kde4_footer()



#original Makefile.am contents follow:

#INCLUDES = $(KOFFICE_INCLUDES) -I$(top_srcdir)/karbon  -I$(top_srcdir)/karbon/core $(all_includes)
#
#kde_module_LTLIBRARIES = karbon_roundcornersplugin.la
#
#karbon_roundcornersplugin_la_SOURCES = roundcornersplugin.cc
#karbon_roundcornersplugin_la_LIBADD = $(LIB_KPARTS) \
#					../../libkarboncommon.la
#
#karbon_roundcornersplugin_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
#
#partpluginsdir   = $(kde_datadir)/karbon/kpartplugins
#partplugins_DATA = roundcornersplugin.rc
#
#METASOURCES = AUTO
#
