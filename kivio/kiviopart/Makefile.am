SUBDIRS = ui kiviosdk tools pics tiles stencils .

INCLUDES = -I$(srcdir)/ui\
	-Iui \
	-I$(srcdir)/kiviosdk\
	-I$(srcdir)/tools\
	$(KOFFICE_INCLUDES)\
	$(KOTEXT_INCLUDES)\
	-I$(top_srcdir)/lib/kformula\
	$(all_includes)


lib_LTLIBRARIES = 
kde_module_LTLIBRARIES = libkiviopart.la

libkiviopart_la_LDFLAGS = -avoid-version -module $(all_libraries) -no-undefined

libkiviopart_la_LIBADD = \
	$(top_builddir)/kivio/kiviopart/tools/libtools.la\
	$(top_builddir)/kivio/kiviopart/kiviosdk/libkiviosdk.la\
	$(top_builddir)/kivio/kiviopart/ui/libui.la \
	$(LIB_KOFFICEUI) $(LIB_KOPAINTER) $(LIB_KOTEXT) $(LIBPYTHON) $(PYTHONLIB)

libkiviopart_la_METASOURCES = AUTO

libkiviopart_la_SOURCES = kivio_canvas.cpp kivio_doc.cpp kivio_factory.cpp 	kivio_tabbar.cpp kivio_view.cpp kivio_map.cpp kivio_page.cpp 	kivio_dlg_pageshow.cpp kivio_zoomaction.cpp kivio_icon_view.cpp 	kivio_config.cpp kivio_spawner_drag.cpp kivio_intra_stencil_data.cpp 	kivio_stackbar.cpp stencilbarbutton.cpp stencilbardockmanager.cpp 	stencilbarmovemanager.cpp tooldockbase.cpp tooldockmanager.cpp 	stencil_text_dlg.cpp kivio_grid_data.cpp kivio_guidelines.cpp 	kivio_stencil_geometry_panel.cpp kivio_viewmanager_panel.cpp 	KIvioViewIface.skel KIvioViewIface.cc KIvioDocIface.skel KIvioDocIface.cc 	KIvioMapIface.skel KIvioMapIface.cc KIvioPageIface.skel KIvioPageIface.cc 	kivio_command.cc KIvioLayerIface.skel KIvioLayerIface.cc kivioglobal.cpp 	kiviostencilsetaction.cpp kivioarrowheadaction.cpp KIvioStencilIface.skel 	KIvioStencilIface.cc kiviodragobject.cpp kolinewidthaction.cpp kolinestyleaction.cpp


kdeinit_LTLIBRARIES = kivio.la
bin_PROGRAMS =

kivio_la_SOURCES = main.cpp

kivio_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)

kivio_la_LIBADD = $(LIB_KOFFICECORE)


kdelnk_DATA = kivio.desktop
kdelnkdir = $(kde_appsdir)/Office


rc_DATA = kivio.rc kivio_readonly.rc
rcdir = $(kde_datadir)/kivio


KDE_ICON = kivio


kivio_view.lo: ui/kivio_protection_panel_base.h \
	ui/kivio_layer_panel_base.h ui/kivio_birdeye_panel_base.h \
	ui/export_page_dialog_base.h  \
	ui/aligndialog.h
kde_services_DATA = kiviopart.desktop
