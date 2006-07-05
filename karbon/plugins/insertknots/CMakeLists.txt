

include_directories( ${CMAKE_SOURCE_DIR}/karbon ${CMAKE_SOURCE_DIR}/karbon/core ${KDE4_INCLUDES}  )


########### next target ###############

set(karbon_insertknotsplugin_PART_SRCS insertknotsplugin.cc )

kde4_automoc(${karbon_insertknotsplugin_PART_SRCS})

kde4_add_plugin(karbon_insertknotsplugin ${karbon_insertknotsplugin_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} karbon_insertknotsplugin )

target_link_libraries(karbon_insertknotsplugin  ${KDE4_KDECORE_LIBS} ${KDE4_KPARTS_LIBS} karboncommon )

install(TARGETS karbon_insertknotsplugin  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  insertknotsplugin.rc  DESTINATION ${DATA_INSTALL_DIR}/karbon/kpartplugins)





#original Makefile.am contents follow:

#INCLUDES = $(KOFFICE_INCLUDES) -I$(top_srcdir)/karbon  -I$(top_srcdir)/karbon/core $(all_includes)
#
#kde_module_LTLIBRARIES = karbon_insertknotsplugin.la
#
#karbon_insertknotsplugin_la_SOURCES = insertknotsplugin.cc
#karbon_insertknotsplugin_la_LIBADD = $(LIB_KPARTS) \
#					../../libkarboncommon.la
#
#karbon_insertknotsplugin_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
#
#partpluginsdir   = $(kde_datadir)/karbon/kpartplugins
#partplugins_DATA = insertknotsplugin.rc
#
#METASOURCES = AUTO
#
