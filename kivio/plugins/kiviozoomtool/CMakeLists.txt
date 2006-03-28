kde4_header()

add_subdirectory( zoom_pics )

include_directories( ${CMAKE_SOURCE_DIR}/kivio/kiviopart/config ${CMAKE_SOURCE_DIR}/kivio/kiviopart ${CMAKE_SOURCE_DIR}/kivio/kiviopart/kiviosdk ${CMAKE_SOURCE_DIR}/kivio/kiviopart/tools ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(kiviozoomtool_PART_SRCS plugin.cpp tool_zoom.cpp )

kde4_automoc(${kiviozoomtool_PART_SRCS})

kde4_add_plugin(kiviozoomtool WITH_PREFIX ${kiviozoomtool_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kiviozoomtool )

target_link_libraries(kiviozoomtool  ${KDE4_KDECORE_LIBS} kotext kopainter kiviocommon )

install_targets(${LIB_INSTALL_DIR}/kde4 kiviozoomtool )


########### install files ###############

install_files( ${DATA_INSTALL_DIR}/kivio/kpartplugins FILES  kiviozoomtool.rc )
install_files( ${SERVICES_INSTALL_DIR} FILES  kiviozoomtool.desktop )

kde4_footer()



#original Makefile.am contents follow:

#SUBDIRS = zoom_pics
#kde_module_LTLIBRARIES =libkiviozoomtool.la
#
#INCLUDES = -I$(top_srcdir)/kivio/kiviopart/config \
#	-I$(top_builddir)/kivio/kiviopart/config \
#	-I$(top_srcdir)/kivio/kiviopart\
#	-I$(top_srcdir)/kivio/kiviopart/kiviosdk\
#	-I$(top_srcdir)/kivio/kiviopart/tools\
#	$(KOFFICE_INCLUDES) $(KOTEXT_INCLUDES) $(all_includes)
#libkiviozoomtool_la_METASOURCES = AUTO
#libkiviozoomtool_la_SOURCES = \
#		plugin.cpp\
#		tool_zoom.cpp
#EXTRA_DIST = \
#		plugin.cpp\
#		plugin.h\
#		kiviozoomtool.desktop\
#		kiviozoomtool.rc\
#		tool_zoom.cpp\
#		tool_zoom.h
#libkiviozoomtool_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
#libkiviozoomtool_la_LIBADD = $(LIB_KOTEXT) $(LIB_KOPAINTER) $(top_builddir)/kivio/kiviopart/libkiviocommon.la
#
#rc_DATA = kiviozoomtool.rc
#rcdir = $(kde_datadir)/kivio/kpartplugins
#
#desktop_DATA = kiviozoomtool.desktop
#desktopdir = $(kde_servicesdir)
#
