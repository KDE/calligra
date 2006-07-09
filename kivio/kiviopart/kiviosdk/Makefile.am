KDE_OPTIONS = nofinal

noinst_LTLIBRARIES =libkiviosdk.la

INCLUDES = -I$(top_srcdir)/kivio/kiviopart/config \
	-I$(top_builddir)/kivio/kiviopart/config \
	-I$(top_srcdir)/kivio/kiviopart\
	$(KOFFICECORE_INCLUDES) $(KOFFICE_INCLUDES) $(KOPAINTER_INCLUDES) $(KOTEXT_INCLUDES) $(all_includes) $(PYTHONINC)

libkiviosdk_la_METASOURCES = AUTO

libkiviosdk_la_SOURCES = \
		kivio_common.cpp\
		kivio_wrap.cpp\
		kivio_py_stencil.cpp\
		kivio_py_stencil_spawner.cpp\
		kivio_fill_style.cpp\
		kivio_gradient.cpp\
		kivio_point.cpp\
		kivio_shape.cpp\
		kivio_shape_data.cpp\
		kivio_sml_stencil.cpp\
		kivio_sml_stencil_spawner.cpp\
		kivio_dia_stencil_spawner.cpp\
		kivio_stencil.cpp\
		kivio_stencil_spawner.cpp\
		kivio_stencil_spawner_info.cpp\
		kivio_stencil_spawner_set.cpp\
		kivio_painter.cpp\
		kivio_screen_painter.cpp\
		kivio_layer.cpp\
		kivio_connector_target.cpp\
		kivio_group_stencil.cpp\
		kivio_connector_point.cpp\
		kivio_plugin_stencil_spawner.cpp\
		kivio_custom_drag_data.cpp\
		kivio_shape_painter.cpp\
		kivio_arrowhead.cpp\
		kivio_base_connector_stencil.cpp\
		kivio_line_style.cpp\
		kivio_text_style.cpp\
		kivio_1d_stencil.cpp \
		diapathparser.cpp \
		kiviopolylineconnector.cpp \
		polylineconnectorspawner.cpp
