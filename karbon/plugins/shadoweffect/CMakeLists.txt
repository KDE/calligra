kde4_header()

include_directories( ${CMAKE_SOURCE_DIR}/karbon ${CMAKE_SOURCE_DIR}/karbon/core ${KDE4_INCLUDES}  )


########### next target ###############

set(karbon_shadoweffectplugin_PART_SRCS shadoweffectplugin.cc vshadowdecorator.cc )

kde4_automoc(${karbon_shadoweffectplugin_PART_SRCS})

kde4_add_plugin(karbon_shadoweffectplugin ${karbon_shadoweffectplugin_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} karbon_shadoweffectplugin )

target_link_libraries(karbon_shadoweffectplugin  ${KDE4_KDECORE_LIBS} ${KDE4_KPARTS_LIBS} kofficeui karboncommon )

install_targets(${PLUGIN_INSTALL_DIR} karbon_shadoweffectplugin )


########### install files ###############

install_files( ${DATA_INSTALL_DIR}/karbon/kpartplugins FILES  shadoweffectplugin.rc )

kde4_footer()



#original Makefile.am contents follow:

#INCLUDES = $(KOFFICE_INCLUDES) $(KOPAINTER_INCLUDES) -I$(top_srcdir)/karbon  -I$(top_srcdir)/karbon/core $(all_includes)
#
#kde_module_LTLIBRARIES = karbon_shadoweffectplugin.la
#
#karbon_shadoweffectplugin_la_SOURCES = shadoweffectplugin.cc vshadowdecorator.cc 
#karbon_shadoweffectplugin_la_LIBADD = $(LIB_KPARTS) $(LIB_KOFFICEUI) \
#					../../libkarboncommon.la
#
#karbon_shadoweffectplugin_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
#
#partpluginsdir   = $(kde_datadir)/karbon/kpartplugins
#partplugins_DATA = shadoweffectplugin.rc
#
#METASOURCES = AUTO
#
