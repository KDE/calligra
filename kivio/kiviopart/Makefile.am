SUBDIRS = config ui kiviosdk tools pics tiles stencils .

INCLUDES = -I$(srcdir)/ui\
	-Iui \
	-I$(srcdir)/config \
	-Iconfig \
	-I$(srcdir)/kiviosdk\
	-I$(srcdir)/tools\
	$(KOFFICE_INCLUDES)\
	$(KOPALETTE_INCLUDES)\
	$(KOTEXT_INCLUDES)\
	-I$(top_srcdir)/lib/kformula\
	$(all_includes)


lib_LTLIBRARIES = libkiviocommon.la

libkiviocommon_la_LDFLAGS = $(all_libraries) -no-undefined

libkiviocommon_la_LIBADD = \
	$(top_builddir)/kivio/kiviopart/tools/libtools.la\
	$(top_builddir)/kivio/kiviopart/kiviosdk/libkiviosdk.la\
	$(top_builddir)/kivio/kiviopart/config/libkivioconfig.la \
	$(top_builddir)/kivio/kiviopart/ui/libui.la \
	$(LIB_KOFFICEUI) $(LIB_KOPAINTER) $(LIB_KOPALETTE) $(LIB_KOTEXT) $(LIBPYTHON) $(PYTHONLIB)

libkiviocommon_la_METASOURCES = AUTO

libkiviocommon_la_SOURCES = kivio_canvas.cpp kivio_doc.cpp kivio_factory.cpp \
	kivio_view.cpp kivio_map.cpp kivio_page.cpp kivio_dlg_pageshow.cpp \
	kivio_zoomaction.cpp kivio_icon_view.cpp kivio_spawner_drag.cpp kivio_intra_stencil_data.cpp \
	kivio_stackbar.cpp stencilbarbutton.cpp stencilbardockmanager.cpp kivio_grid_data.cpp \
	kivio_stencil_geometry_panel.cpp KIvioViewIface.skel KIvioViewIface.cc KIvioDocIface.skel \
	KIvioDocIface.cc KIvioMapIface.skel KIvioMapIface.cc KIvioPageIface.skel \
	KIvioPageIface.cc kivio_command.cc KIvioLayerIface.skel KIvioLayerIface.cc \
	kivioglobal.cpp kiviostencilsetaction.cpp kivioarrowheadaction.cpp \
	KIvioStencilIface.skel KIvioStencilIface.cc kiviodragobject.cpp kiviostencilsetinstaller.cpp \
	addstenciltool.cpp

kde_module_LTLIBRARIES = libkiviopart.la
libkiviopart_la_LDFLAGS = -avoid-version $(all_libraries) -module
libkiviopart_la_LIBADD  = libkiviocommon.la
libkiviopart_la_SOURCES = kivio_part_init.cc

dummy.cpp:
	echo > dummy.cpp

kdeinit_LTLIBRARIES = kivio.la
bin_PROGRAMS =

kivio_la_SOURCES = main.cpp

kivio_la_LDFLAGS = $(all_libraries) -module -avoid-version

kivio_la_LIBADD = $(LIB_KOFFICECORE)


#kdelnk_DATA = kivio.desktop
#kdelnkdir = $(kde_appsdir)/Office
xdg_apps_DATA = kivio.desktop

kde_services_DATA = kiviopart.desktop

rc_DATA = kivio.rc kivio_readonly.rc
rcdir = $(kde_datadir)/kivio

KDE_ICON = kivio

kivio_view.lo: ui/kivio_protection_panel_base.h \
	ui/kivio_layer_panel_base.h ui/kivio_birdeye_panel_base.h \
	ui/export_page_dialog_base.h  \
	ui/aligndialog.h
noinst_HEADERS = kiviostencilsetinstaller.h addstenciltool.h
