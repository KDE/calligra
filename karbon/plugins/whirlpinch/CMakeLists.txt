

include_directories( ${CMAKE_SOURCE_DIR}/karbon ${CMAKE_SOURCE_DIR}/karbon/core ${KDE4_INCLUDES}  )


########### next target ###############

set(karbon_whirlpinchplugin_PART_SRCS whirlpinchplugin.cc )

kde4_automoc(${karbon_whirlpinchplugin_PART_SRCS})

kde4_add_plugin(karbon_whirlpinchplugin ${karbon_whirlpinchplugin_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} karbon_whirlpinchplugin )

target_link_libraries(karbon_whirlpinchplugin  ${KDE4_KDECORE_LIBS} ${KDE4_KPARTS_LIBS} kopainter karboncommon )

install(TARGETS karbon_whirlpinchplugin  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  whirlpinchplugin.rc  DESTINATION ${DATA_INSTALL_DIR}/karbon/kpartplugins)





#original Makefile.am contents follow:

#INCLUDES = $(KOFFICE_INCLUDES) -I$(top_srcdir)/karbon -I$(top_srcdir)/karbon/core $(KOPAINTER_INCLUDES) $(all_includes)
#
#kde_module_LTLIBRARIES = karbon_whirlpinchplugin.la
#
#karbon_whirlpinchplugin_la_SOURCES = whirlpinchplugin.cc
#karbon_whirlpinchplugin_la_LIBADD = $(LIB_KPARTS) $(LIB_KOPAINTER) \
#				../../libkarboncommon.la
#
#
#karbon_whirlpinchplugin_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
#
#partpluginsdir   = $(kde_datadir)/karbon/kpartplugins
#partplugins_DATA = whirlpinchplugin.rc
#
#METASOURCES = AUTO
#
