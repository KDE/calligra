

include_directories( ${CMAKE_SOURCE_DIR}/kivio/kiviopart/config ${CMAKE_SOURCE_DIR}/kivio/kiviopart ${CMAKE_SOURCE_DIR}/kivio/kiviopart/kiviosdk ${CMAKE_SOURCE_DIR}/kivio/kiviopart/tools ${KDE4_INCLUDES}  )


########### next target ###############

set(kiviotexttool_PART_SRCS plugin.cpp tool_text.cpp stenciltexteditor.cpp )

kde4_automoc(${kiviotexttool_PART_SRCS})

kde4_add_ui3_files(kiviotexttool_PART_SRCS kivio_stenciltexteditorui.ui )

kde4_add_plugin(kiviotexttool WITH_PREFIX ${kiviotexttool_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kiviotexttool )

target_link_libraries(kiviotexttool  ${KDE4_KDECORE_LIBS} kotext kopainter kiviocommon )

install(TARGETS kiviotexttool  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kivio_text.xpm kivio_text_cursor.xpm  DESTINATION ${DATA_INSTALL_DIR}/kivio/pics)
install( FILES  Text.sml Text.xpm  DESTINATION ${DATA_INSTALL_DIR}/kivio/autoloadStencils)
install( FILES  kiviotexttool.rc  DESTINATION ${DATA_INSTALL_DIR}/kivio/kpartplugins)
install( FILES  kiviotexttool.desktop  DESTINATION ${SERVICES_INSTALL_DIR})





#original Makefile.am contents follow:

#kde_module_LTLIBRARIES =libkiviotexttool.la
#
#
#INCLUDES = -I$(top_srcdir)/kivio/kiviopart/config \
#	-I$(top_builddir)/kivio/kiviopart/config \
#	-I$(top_srcdir)/kivio/kiviopart\
#	-I$(top_srcdir)/kivio/kiviopart/kiviosdk\
#	-I$(top_srcdir)/kivio/kiviopart/tools\
#	$(KOFFICE_INCLUDES) $(KOTEXT_INCLUDES) $(all_includes)
#libkiviotexttool_la_METASOURCES = AUTO
#libkiviotexttool_la_SOURCES = \
#		plugin.cpp\
#		tool_text.cpp kivio_stenciltexteditorui.ui stenciltexteditor.cpp
#EXTRA_DIST = \
#		plugin.h\
#		plugin.cpp\
#		kiviotexttool.desktop\
#		kiviotexttool.rc\
#		tool_text.cpp\
#		tool_text.h\
#		kivio_text.xpm\
#		kivio_text_cursor.xpm\
#		Text.sml\
#		Text.xpm
#libkiviotexttool_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
#libkiviotexttool_la_LIBADD =$(LIB_KOTEXT) $(LIB_KOPAINTER) $(top_builddir)/kivio/kiviopart/libkiviocommon.la
#
#rc_DATA = kiviotexttool.rc
#rcdir = $(kde_datadir)/kivio/kpartplugins
#
#desktop_DATA = kiviotexttool.desktop
#desktopdir = $(kde_servicesdir)
#
#kiviopics_DATA = kivio_text.xpm kivio_text_cursor.xpm
#kiviopicsdir = $(kde_datadir)/kivio/pics
#
#stencil_DATA = Text.sml Text.xpm
#stencildir = $(kde_datadir)/kivio/autoloadStencils
