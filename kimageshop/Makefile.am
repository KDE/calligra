INCLUDES  = $(MICO_INCLUDES) $(KOFFICE_INCLUDES) $(all_includes) 
LDFLAGS   = $(all_libraries) $(MICO_LDFLAGS) $(KDE_RPATH)
LDADD     = $(LIB_KOFFICEUI) $(LIB_KIMGIO) 

SUBDIRS   = pics data

####### Files

bin_PROGRAMS = kimageshop

kimageshop_SOURCES = integerwidget.cc kimageshop.idl kimageshop_main.cc \
	kimageshop_shell.cc kimageshop_doc.cc kimageshop_view.cc \
	kimageshop_undo.cc kimageshop_config.cc kvector.cc canvas.cc canvasview.cc \
	layer.cc layerview.cc layerdlg.cc brush.cc \
	channel.cc channelview.cc misc.cc tool.cc movetool.cc \
	brushtool.cc zoomtool.cc kfloatingdialog.cc \
	colordialog.cc kfloatingtabdialog.cc kcolor.cc \
	brusheswidget.cc brushdialog.cc \
	iconchooser.cc kfloatingdialogowner.cc preferencesdlg.cc \
	gradientdlg.cc gradienteditordlg.cc gradientview.cc \
	newdialog.cc colorframe.cc colorslider.cc

noinst_HEADERS = kimageshop_main.h kimageshop_shell.h \
	kimageshop_doc.h kimageshop_view.h kimageshop_undo.h kimageshop_config.h \
	kvector.h zoomtool.h kfloatingdialog.h kcolor.h \
	canvas.h canvasview.h layer.h layerview.h layerdlg.h \
	brush.h channel.h misc.h tool.h movetool.h brushtool.h \
	colordialog.h channelview.h kfloatingtabdialog.h \
	integerwidget.h brusheswidget.h brushdialog.h \
	iconchooser.h kfloatingdialogowner.h preferencesdlg.h \
	gradientdlg.h gradienteditordlg.h gradientview.h \
	newdialog.h colorframe.h colorslider.h

METASOURCES = kimageshop_main.moc kimageshop_shell.moc \
	kimageshop_doc.moc kimageshop_view.moc kimageshop_config.moc \
	canvasview.moc \
	canvas.moc layer.moc layerview.moc layerdlg.moc \
	kfloatingdialog.moc colordialog.moc channelview.moc \
	kfloatingtabdialog.moc brusheswidget.moc brushdialog.moc \
	iconchooser.moc integerwidget.moc preferencesdlg.moc \
	gradientdlg.moc gradienteditordlg.moc gradientview.moc \
	newdialog.moc colorframe.moc colorslider.moc

idl_DATA = kimageshop.idl

kdelnk_DATA = kimageshop.desktop
kdelnkdir = $(kde_appsdir)/KOffice

####### Build rules

IDL_FLAGS = --query-server-for-narrow --relative-paths -I$(top_srcdir)/lib/kofficecore -I$(top_srcdir)/lib/store -I$(interfacedir) -I$(idldir)

messages:
	(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot *.cc && mv messages.po ../po/kimageshop.pot

