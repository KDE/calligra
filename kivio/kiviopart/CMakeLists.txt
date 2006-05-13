

add_subdirectory( config )
add_subdirectory( ui )
add_subdirectory( kiviosdk )
add_subdirectory( tools )
add_subdirectory( pics )
add_subdirectory( tiles )
add_subdirectory( stencils )

include_directories( ${CMAKE_SOURCE_DIR}/lib/kformula ${KDE4_INCLUDES}  )


########### next target ###############

set(kiviocommon_LIB_SRCS 
   kivio_canvas.cpp 
   kivio_doc.cpp 
   kivio_factory.cpp 
   kivio_view.cpp 
   kivio_map.cpp 
   kivio_page.cpp 
   kivio_dlg_pageshow.cpp 
   kivio_zoomaction.cpp 
   kivio_icon_view.cpp 
   kivio_spawner_drag.cpp 
   kivio_intra_stencil_data.cpp 
   kivio_stackbar.cpp 
   stencilbarbutton.cpp 
   stencilbardockmanager.cpp 
   kivio_grid_data.cpp 
   kivio_stencil_geometry_panel.cpp 
   KIvioViewIface.cc 
   KIvioDocIface.cc 
   KIvioMapIface.cc 
   KIvioPageIface.cc 
   kivio_command.cc 
   KIvioLayerIface.cc 
   kivioglobal.cpp 
   kiviostencilsetaction.cpp 
   kivioarrowheadaction.cpp 
   KIvioStencilIface.cc 
   kiviodragobject.cpp 
   kiviostencilsetinstaller.cpp 
   addstenciltool.cpp )

kde4_automoc(${kiviocommon_LIB_SRCS})

kde4_add_dcop_skels(kiviocommon_LIB_SRCS 
   KIvioViewIface.h 
   KIvioDocIface.h 
   KIvioMapIface.h 
   KIvioPageIface.h 
   KIvioLayerIface.h 
   KIvioStencilIface.h )

kde4_add_library(kiviocommon SHARED ${kiviocommon_LIB_SRCS})

target_link_libraries(kiviocommon  ${KDE4_KDECORE_LIBS} tools kiviosdk kivioconfig ui kofficeui kopainter kopalette kotext )

set_target_properties(kiviocommon PROPERTIES VERSION 1.0.0 SOVERSION 1 )
install_targets(${LIB_INSTALL_DIR} kiviocommon )


########### next target ###############

set(kiviopart_PART_SRCS kivio_part_init.cc )

kde4_automoc(${kiviopart_PART_SRCS})

kde4_add_plugin(kiviopart WITH_PREFIX ${kiviopart_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kiviopart )

target_link_libraries(kiviopart  ${KDE4_KDECORE_LIBS} kiviocommon )

install_targets(${PLUGIN_INSTALL_DIR} kiviopart )


########### next target ###############

set(kivio_KDEINIT_SRCS main.cpp )

kde4_automoc(${kivio_KDEINIT_SRCS})

kde4_add_kdeinit_executable( kivio ${kivio_KDEINIT_SRCS})

target_link_libraries(kdeinit_kivio  ${KDE4_KDECORE_LIBS} kofficecore )

install_targets(${LIB_INSTALL_DIR} kdeinit_kivio )

target_link_libraries( kivio kdeinit_kivio )
install_targets(/bin kivio )

########### install files ###############

install_files( ${XDG_APPS_DIR} FILES  kivio.desktop )
install_files( ${DATA_INSTALL_DIR}/kivio FILES  kivio.rc kivio_readonly.rc )
install_files( ${SERVICES_INSTALL_DIR} FILES  kiviopart.desktop )

kde4_install_icons( ${ICON_INSTALL_DIR}  hicolor )





#original Makefile.am contents follow:

#SUBDIRS = config ui kiviosdk tools pics tiles stencils .
#
#INCLUDES = -I$(srcdir)/ui\
#	-Iui \
#	-I$(srcdir)/config \
#	-Iconfig \
#	-I$(srcdir)/kiviosdk\
#	-I$(srcdir)/tools\
#	$(KOFFICE_INCLUDES)\
#	$(KOPALETTE_INCLUDES)\
#	$(KOTEXT_INCLUDES)\
#	-I$(top_srcdir)/lib/kformula\
#	$(all_includes)
#
#
#lib_LTLIBRARIES = libkiviocommon.la
#
#libkiviocommon_la_LDFLAGS = $(all_libraries) -no-undefined
#
#libkiviocommon_la_LIBADD = \
#	$(top_builddir)/kivio/kiviopart/tools/libtools.la\
#	$(top_builddir)/kivio/kiviopart/kiviosdk/libkiviosdk.la\
#	$(top_builddir)/kivio/kiviopart/config/libkivioconfig.la \
#	$(top_builddir)/kivio/kiviopart/ui/libui.la \
#	$(LIB_KOFFICEUI) $(LIB_KOPAINTER) $(LIB_KOPALETTE) $(LIB_KOTEXT) $(LIBPYTHON) $(PYTHONLIB)
#
#libkiviocommon_la_METASOURCES = AUTO
#
#libkiviocommon_la_SOURCES = kivio_canvas.cpp kivio_doc.cpp kivio_factory.cpp \
#	kivio_view.cpp kivio_map.cpp kivio_page.cpp kivio_dlg_pageshow.cpp \
#	kivio_zoomaction.cpp kivio_icon_view.cpp kivio_spawner_drag.cpp kivio_intra_stencil_data.cpp \
#	kivio_stackbar.cpp stencilbarbutton.cpp stencilbardockmanager.cpp kivio_grid_data.cpp \
#	kivio_stencil_geometry_panel.cpp KIvioViewIface.skel KIvioViewIface.cc KIvioDocIface.skel \
#	KIvioDocIface.cc KIvioMapIface.skel KIvioMapIface.cc KIvioPageIface.skel \
#	KIvioPageIface.cc kivio_command.cc KIvioLayerIface.skel KIvioLayerIface.cc \
#	kivioglobal.cpp kiviostencilsetaction.cpp kivioarrowheadaction.cpp \
#	KIvioStencilIface.skel KIvioStencilIface.cc kiviodragobject.cpp kiviostencilsetinstaller.cpp \
#	addstenciltool.cpp
#
#kde_module_LTLIBRARIES = libkiviopart.la
#libkiviopart_la_LDFLAGS = -avoid-version $(all_libraries) -module
#libkiviopart_la_LIBADD  = libkiviocommon.la
#libkiviopart_la_SOURCES = kivio_part_init.cc
#
#dummy.cpp:
#	echo > dummy.cpp
#
#kdeinit_LTLIBRARIES = kivio.la
#bin_PROGRAMS =
#
#kivio_la_SOURCES = main.cpp
#
#kivio_la_LDFLAGS = $(all_libraries) -module -avoid-version
#
#kivio_la_LIBADD = $(LIB_KOFFICECORE)
#
#
##kdelnk_DATA = kivio.desktop
##kdelnkdir = $(kde_appsdir)/Office
#xdg_apps_DATA = kivio.desktop
#
#kde_services_DATA = kiviopart.desktop
#
#rc_DATA = kivio.rc kivio_readonly.rc
#rcdir = $(kde_datadir)/kivio
#
#KDE_ICON = kivio
#
#kivio_view.lo: ui/kivio_protection_panel_base.h \
#	ui/kivio_layer_panel_base.h ui/kivio_birdeye_panel_base.h \
#	ui/export_page_dialog_base.h  \
#	ui/aligndialog.h
#noinst_HEADERS = kiviostencilsetinstaller.h addstenciltool.h
