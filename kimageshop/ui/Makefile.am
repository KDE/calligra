INCLUDES = -I$(srcdir)/../core $(KOFFICE_INCLUDES) $(all_includes)

noinst_LTLIBRARIES = libkisui.la

libkisui_la_SOURCES = colorframe.cc colorslider.cc \
	iconchooser.cc integerwidget.cc blendchooser.cc \
	kis_dlg_gradient.cc kis_dlg_brush.cc brusheswidget.cc \
	kis_dlg_color.cc kis_dlg_gradienteditor.cc gradientview.cc \
	kis_dlg_new.cc kis_dlg_preferences.cc layerview.cc channelview.cc \
	kis_dlg_layer.cc kfloatingdialog.cc kfloatingtabdialog.cc kfloatingdialogowner.cc

libkisui_la_METASOURCES = colorframe.moc colorslider.moc \
	iconchooser.moc integerwidget.moc blendchooser.moc \
	kis_dlg_brush.moc brusheswidget.moc \
	kis_dlg_color.moc kis_dlg_gradient.moc kis_dlg_gradienteditor.moc gradientview.moc \
	kis_dlg_new.moc kis_dlg_preferences.moc layerview.moc channelview.moc kis_dlg_layer.moc \
	kfloatingdialog.moc kfloatingtabdialog.moc

#include_HEADERS =

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot *.cc *.h && mv messages.po ../../po/libkisui.pot
