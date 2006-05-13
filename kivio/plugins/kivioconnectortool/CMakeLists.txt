

add_subdirectory( straight_connector )

include_directories( ${CMAKE_SOURCE_DIR}/kivio/kiviopart/config ${CMAKE_SOURCE_DIR}/kivio/kiviopart ${CMAKE_SOURCE_DIR}/kivio/kiviopart/kiviosdk ${CMAKE_SOURCE_DIR}/kivio/kiviopart/tools ${CMAKE_SOURCE_DIR}/kivio/plugins/kivioconnectortool/straight_connector ${CMAKE_SOURCE_DIR}/kivio/plugins/kivioconnectortool/polyline_connector ${KDE4_INCLUDES}  )


########### next target ###############

set(kivioconnectortool_PART_SRCS plugin.cpp tool_connector.cpp )

kde4_automoc(${kivioconnectortool_PART_SRCS})

kde4_add_plugin(kivioconnectortool WITH_PREFIX ${kivioconnectortool_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kivioconnectortool )

target_link_libraries(kivioconnectortool  ${KDE4_KDECORE_LIBS} kopainter kiviocommon )

install_targets(${PLUGIN_INSTALL_DIR} kivioconnectortool )


########### install files ###############

install_files( ${DATA_INSTALL_DIR}/kivio/pics FILES  kivio_connector_cursor1.png kivio_connector_cursor2.png )
install_files( ${DATA_INSTALL_DIR}/kivio/icons FILES  )
install_files( ${DATA_INSTALL_DIR}/kivio/kpartplugins FILES  kivioconnectortool.rc )
install_files( ${SERVICES_INSTALL_DIR} FILES  kivioconnectortool.desktop )





#original Makefile.am contents follow:

#SUBDIRS = straight_connector .
#kde_module_LTLIBRARIES =libkivioconnectortool.la
#
#
#INCLUDES = -I$(top_srcdir)/kivio/kiviopart/config \
#	-I$(top_builddir)/kivio/kiviopart/config \
#	-I$(top_srcdir)/kivio/kiviopart\
#	-I$(top_srcdir)/kivio/kiviopart/kiviosdk\
#	-I$(top_srcdir)/kivio/kiviopart/tools\
#	-I$(top_srcdir)/kivio/plugins/kivioconnectortool/straight_connector \
#	-I$(top_srcdir)/kivio/plugins/kivioconnectortool/polyline_connector \
#	$(KOFFICE_INCLUDES) $(KOTEXT_INCLUDES) $(all_includes)
#libkivioconnectortool_la_METASOURCES = AUTO
#libkivioconnectortool_la_SOURCES = \
#		plugin.cpp\
#		tool_connector.cpp
#EXTRA_DIST = \
#		plugin.h\
#		plugin.cpp\
#		kivio_connector.xpm\
#		tool_connector.h\
#		tool_connector.cpp\
#		kivioconnectortool.rc\
#		kivioconnectortool.desktop\
#		kivio_connector_cursor2.xpm\
#		kivio_connector_cursor1.xpm
#
#libkivioconnectortool_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
#libkivioconnectortool_la_LIBADD = $(LIB_KOPAINTER) $(top_builddir)/kivio/kiviopart/libkiviocommon.la
#
#rc_DATA = kivioconnectortool.rc
#rcdir = $(kde_datadir)/kivio/kpartplugins
#
#desktop_DATA = kivioconnectortool.desktop
#desktopdir = $(kde_servicesdir)
#
#kiviopics_DATA = kivio_connector_cursor1.png kivio_connector_cursor2.png
#kiviopicsdir = $(kde_datadir)/kivio/pics
#
#kivioicondir = $(kde_datadir)/kivio/icons
#kivioicon_ICON = AUTO
