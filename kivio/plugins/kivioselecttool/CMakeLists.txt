kde4_header()

add_subdirectory( select_pics )

include_directories( ${CMAKE_SOURCE_DIR}/kivio/kiviopart/config ${CMAKE_SOURCE_DIR}/kivio/kiviopart ${CMAKE_SOURCE_DIR}/kivio/kiviopart/kiviosdk ${CMAKE_SOURCE_DIR}/kivio/kiviopart/tools ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(kivioselecttool_PART_SRCS plugin.cpp tool_select.cpp )

kde4_automoc(${kivioselecttool_PART_SRCS})

kde4_add_plugin(kivioselecttool WITH_PREFIX ${kivioselecttool_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kivioselecttool )

target_link_libraries(kivioselecttool  ${KDE4_KDECORE_LIBS} kotext kopainter kiviocommon )

install_targets(${PLUGIN_INSTALL_DIR} kivioselecttool )


########### install files ###############

install_files( ${DATA_INSTALL_DIR}/kivio/kpartplugins FILES  kivioselecttool.rc )
install_files( ${SERVICES_INSTALL_DIR} FILES  kivioselecttool.desktop )

kde4_footer()



#original Makefile.am contents follow:

#SUBDIRS = select_pics
#kde_module_LTLIBRARIES =libkivioselecttool.la
#
#
#INCLUDES = -I$(top_srcdir)/kivio/kiviopart/config \
#	-I$(top_builddir)/kivio/kiviopart/config \
#	-I$(top_srcdir)/kivio/kiviopart\
#	-I$(top_srcdir)/kivio/kiviopart/kiviosdk\
#	-I$(top_srcdir)/kivio/kiviopart/tools\
#	$(KOFFICE_INCLUDES) $(KOTEXT_INCLUDES) $(all_includes)
#libkivioselecttool_la_METASOURCES = AUTO
#libkivioselecttool_la_SOURCES = \
#		plugin.cpp\
#		tool_select.cpp
#EXTRA_DIST = \
#		plugin.cpp\
#		plugin.h\
#		kivioselecttool.desktop\
#		tool_select.cpp\
#		tool_select.h\
#		kivioselecttool.rc
#libkivioselecttool_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
#libkivioselecttool_la_LIBADD = $(LIB_KOTEXT) $(LIB_KOPAINTER) $(top_builddir)/kivio/kiviopart/libkiviocommon.la
#
#rc_DATA = kivioselecttool.rc
#rcdir = $(kde_datadir)/kivio/kpartplugins
#
#desktop_DATA = kivioselecttool.desktop
#desktopdir = $(kde_servicesdir)
#
