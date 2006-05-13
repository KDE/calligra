

include_directories( ${CMAKE_SOURCE_DIR}/kivio/kiviopart/config ${CMAKE_SOURCE_DIR}/kivio/kiviopart ${CMAKE_SOURCE_DIR}/kivio/kiviopart/kiviosdk ${CMAKE_SOURCE_DIR}/kivio/kiviopart/tools ${KDE4_INCLUDES}  )


########### next target ###############

set(kiviotargettool_PART_SRCS kiviotargettool.cpp kiviotargettoolfactory.cpp )

kde4_automoc(${kiviotargettool_PART_SRCS})

kde4_add_plugin(kiviotargettool WITH_PREFIX ${kiviotargettool_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kiviotargettool )

target_link_libraries(kiviotargettool  ${KDE4_KDECORE_LIBS} kotext kopainter kiviocommon )

install_targets(${PLUGIN_INSTALL_DIR} kiviotargettool )


########### install files ###############

install_files( ${DATA_INSTALL_DIR}/kivio/kpartplugins FILES  kiviotargettool.rc )
install_files( ${SERVICES_INSTALL_DIR} FILES  kiviotargettool.desktop )





#original Makefile.am contents follow:

#INCLUDES = -I$(top_srcdir)/kivio/kiviopart/config \
#	-I$(top_builddir)/kivio/kiviopart/config \
#	-I$(top_srcdir)/kivio/kiviopart\
#	-I$(top_srcdir)/kivio/kiviopart/kiviosdk\
#	-I$(top_srcdir)/kivio/kiviopart/tools\
#	$(KOFFICE_INCLUDES) $(KOTEXT_INCLUDES) $(all_includes)
#
#METASOURCES = AUTO
#
#kde_module_LTLIBRARIES =  libkiviotargettool.la
#libkiviotargettool_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
#libkiviotargettool_la_LIBADD = $(LIB_KOTEXT) $(LIB_KOPAINTER) $(top_builddir)/kivio/kiviopart/libkiviocommon.la
#libkiviotargettool_la_SOURCES = kiviotargettool.cpp kiviotargettoolfactory.cpp
#
#noinst_HEADERS =  kiviotargettool.h kiviotargettoolfactory.h
#
#rc_DATA = kiviotargettool.rc
#rcdir = $(kde_datadir)/kivio/kpartplugins
#
#desktop_DATA = kiviotargettool.desktop
#desktopdir = $(kde_servicesdir)
