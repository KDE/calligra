#1########################################################!@#$
####   Makefile.am generated automatically by KDEStudio   ####
#### WARNING! All changes made in this part will be lost! ####
##############################################################
SUBDIRS = ui kiviosdk tklib tools pics tiles stencils 
lib_LTLIBRARIES =libkiviopart.la


libkiviopart_la_LDFLAGS = -version-info 
libkiviopart_la_LIBADD = \
		$(top_builddir)/kivio/kiviopart/tools/libtools.la\
		$(top_builddir)/kivio/kiviopart/tklib/libtklib.la\
		$(top_builddir)/kivio/kiviopart/kiviosdk/libkiviosdk.la\
		$(top_builddir)/kivio/kiviopart/ui/libui.la

INCLUDES = -I$(srcdir)/ui\
	-Iui\
	-I$(srcdir)/kiviosdk\
	-I$(srcdir)/tklib\
	-I$(srcdir)/tools\
	$(KOFFICE_INCLUDES)\
        -I$(top_srcdir)/lib/kformula\
	$(all_includes)

libkiviopart_la_METASOURCES = AUTO
libkiviopart_la_SOURCES = \
		main.cpp\
		kivio_canvas.cpp\
		kivio_doc.cpp\
		kivio_factory.cpp\
		kivio_shell.cpp\
		kivio_tabbar.cpp\
		kivio_view.cpp\
		kivio_map.cpp\
		kivio_page.cpp\
		kivio_dlg_pagename.cpp\
		kivio_dlg_pageshow.cpp\
		kivio_ruler.cpp\
		kivio_zoomaction.cpp\
		add_spawner_set_dlg.cpp\
		kivio_icon_view.cpp\
		kivio_config.cpp\
		kivio_spawner_drag.cpp\
		kivio_intra_stencil_data.cpp\
		toolbarseparator.cpp\
		kivio_stackbar.cpp\
		stencilbarbutton.cpp\
		stencilbardockmanager.cpp\
		stencilbarmovemanager.cpp\
		kivio_lineendsaction.cpp\
		tooldockbase.cpp\
		tooldockmanager.cpp\
		kivio_paragraph_action.cpp\
		stencil_text_dlg.cpp\
		kivio_grid_data.cpp\
		kivio_guidelines.cpp\
		kivio_stencil_geometry_panel.cpp\
		kivio_viewmanager_panel.cpp\
		kivio_layer_panel.cpp\
		kivio_dlg_layername.cpp
EXTRA_DIST = \
		main.cpp\
		kivio_canvas.cpp\
		kivio_canvas.h\
		kivio_doc.cpp\
		kivio_doc.h\
		kivio_factory.cpp\
		kivio_factory.h\
		kivio_shell.cpp\
		kivio_shell.h\
		kivio_tabbar.cpp\
		kivio_tabbar.h\
		kivio_view.cpp\
		kivio_view.h\
		kivio_map.cpp\
		kivio_map.h\
		kivio_page.cpp\
		kivio_page.h\
		kivio_dlg_pagename.cpp\
		kivio_dlg_pagename.h\
		kivio.desktop\
		x-kivio.desktop\
		kivio.rc\
		kivio_dlg_pageshow.cpp\
		kivio_dlg_pageshow.h\
		kivio_ruler.cpp\
		kivio_ruler.h\
		kivio_zoomaction.cpp\
		kivio_zoomaction.h\
		add_spawner_set_dlg.cpp\
		add_spawner_set_dlg.h\
		kivio_icon_view.cpp\
		kivio_icon_view.h\
		kivio_config.cpp\
		kivio_config.h\
		kivio_spawner_drag.cpp\
		kivio_spawner_drag.h\
		kivio_intra_stencil_data.cpp\
		kivio_intra_stencil_data.h\
		toolbarseparator.cpp\
		toolbarseparator.h\
		kivio_stackbar.cpp\
		kivio_stackbar.h\
		stencilbarbutton.cpp\
		stencilbarbutton.h\
		stencilbardockmanager.cpp\
		stencilbardockmanager.h\
		stencilbarmovemanager.cpp\
		stencilbarmovemanager.h\
		kivio_lineendsaction.cpp\
		kivio_lineendsaction.h\
		tooldockbase.cpp\
		tooldockbase.h\
		tooldockmanager.cpp\
		tooldockmanager.h\
		kivio_lineendspix.h\
		kivio_paragraph_action.h\
		kivio_paragraph_action.cpp\
		stencil_text_dlg.cpp\
		stencil_text_dlg.h\
		lo16-app-kivio.png\
		lo22-app-kivio.png\
		lo32-app-kivio.png\
		hi48-app-kivio.png\
		hi16-app-kivio.png\
		hi22-app-kivio.png\
		hi32-app-kivio.png\
		kivio_grid_data.cpp\
		kivio_grid_data.h\
		kivio_guidelines.cpp\
		kivio_guidelines.h\
		kivio_stencil_geometry_panel.cpp\
		kivio_stencil_geometry_panel.h\
		kivio_viewmanager_panel.cpp\
		kivio_viewmanager_panel.h\
		kivio_layer_panel.cpp\
		kivio_layer_panel.h\
		kivio_dlg_layername.cpp\
		kivio_dlg_layername.h
##############################################################
####                  END KDEStudio part                  ####
#2########################################################!@#$
lib_LTLIBRARIES =  libkiviopart.la kivio.la
kivio_la_SOURCES = main.cpp
kivio_la_LDFLAGS = $(all_libraries) -module -avoid-version
kivio_la_LIBADD = libkiviopart.la

libkiviopart_la_LDFLAGS = -avoid-version -module $(LIB_KOFFICEUI) $(all_libraries)

bin_PROGRAMS = kiviopart
kiviopart_SOURCES = dummy.cpp
kiviopart_LDFLAGS = $(all_libraries) $(KDE_RPATH)
kiviopart_LDADD = kivio.la

dummy.cpp:
	echo > dummy.cpp

kdelnk_DATA = kivio.desktop
kdelnkdir = $(kde_appsdir)/Office

kdemime_DATA = x-kivio.desktop
kdemimedir = $(kde_mimedir)/application

rc_DATA = kivio.rc
rcdir = $(kde_datadir)/kivio

KDE_ICON = kivio
