kde4_header()

add_subdirectory( sml_connector )

include_directories( ${CMAKE_SOURCE_DIR}/kivio/kiviopart/config ${CMAKE_SOURCE_DIR}/kivio/kiviopart ${CMAKE_SOURCE_DIR}/kivio/kiviopart/kiviosdk ${CMAKE_SOURCE_DIR}/kivio/kiviopart/tools ${CMAKE_SOURCE_DIR}/kivio/plugins/kiviosmlconnector/sml_connector ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(kiviosmlconnector_PART_SRCS plugin.cpp tool_connector.cpp )

kde4_automoc(${kiviosmlconnector_PART_SRCS})

kde4_add_plugin(kiviosmlconnector WITH_PREFIX ${kiviosmlconnector_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kiviosmlconnector )

target_link_libraries(kiviosmlconnector  ${KDE4_KDECORE_LIBS} kopainter kiviocommon )

install_targets(${LIB_INSTALL_DIR}/kde4 kiviosmlconnector )


########### install files ###############

install_files( ${DATA_INSTALL_DIR}/kivio/kpartplugins FILES  kiviosmlconnector.rc )
install_files( ${SERVICES_INSTALL_DIR} FILES  kiviosmlconnector.desktop )

kde4_footer()



#original Makefile.am contents follow:

#SUBDIRS=. sml_connector
#kde_module_LTLIBRARIES =libkiviosmlconnector.la
#
#INCLUDES = -I$(top_srcdir)/kivio/kiviopart/config \
#	-I$(top_builddir)/kivio/kiviopart/config \
#	-I$(top_srcdir)/kivio/kiviopart\
#	-I$(top_srcdir)/kivio/kiviopart/kiviosdk\
#	-I$(top_srcdir)/kivio/kiviopart/tools\
#	-I$(top_srcdir)/kivio/plugins/kiviosmlconnector/sml_connector \
#	$(KOFFICE_INCLUDES) $(KOTEXT_INCLUDES) $(all_includes)
#
#METASOURCES = AUTO
#libkiviosmlconnector_la_SOURCES = plugin.cpp tool_connector.cpp
#libkiviosmlconnector_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)
#libkiviosmlconnector_la_LIBADD = $(LIB_KOPAINTER) $(top_builddir)/kivio/kiviopart/libkiviocommon.la
#
#
#
#rc_DATA = kiviosmlconnector.rc
#rcdir = $(kde_datadir)/kivio/kpartplugins
#
#desktop_DATA = kiviosmlconnector.desktop
#desktopdir = $(kde_servicesdir)
#
