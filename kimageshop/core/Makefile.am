INCLUDES  = -I$(srcdir)/../ui -I$(srcdir)/../tools $(KOFFICE_INCLUDES) -I$(interfacedir) $(all_includes)

noinst_LTLIBRARIES = libkiscore.la

libkiscore_la_SOURCES = kis_factory.cc kis_shell.cc kis_doc.cc kis_view.cc \
	kis_image.cc kis_config.cc kis_undo.cc kis_tabbar.cc kis_brush.cc \
	kis_channeldata.cc kis_util.cc kis_layer.cc kis_tool.cc kis_vec.cc kis_color.cc \
	kis_canvas.cc kis_cursor.cc

libkiscore_la_METASOURCES = kis_shell.moc kis_doc.moc kis_view.moc \
	kis_factory.moc kis_config.moc kis_layer.moc kis_tool.moc \
	kis_canvas.moc kis_image.moc kis_tabbar.moc

#include_HEADERS =

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot *.cc *.h && mv messages.po ../../po/libkiscore.pot
