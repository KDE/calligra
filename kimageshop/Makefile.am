
INCLUDES  = $(MICO_INCLUDES) $(KOFFICE_INCLUDES) $(all_includes) 
LDFLAGS   = $(all_libraries) $(MICO_LDFLAGS) $(KDE_RPATH)
LDADD     = $(LIB_KOFFICEUI) $(LIB_KIMGIO) 

SUBDIRS   = pics data

####### Files

bin_PROGRAMS = kimageshop

kimageshop_SOURCES = kimageshop.idl kimageshop_main.cc kimageshop_shell.cc \
	kimageshop_doc.cc kimageshop_view.cc \
	canvas.cc canvasview.cc layer.cc layerview.cc layerdlg.cc \
	brush.cc channel.cc misc.cc tool.cc movetool.cc brushtool.cc kvector.cc

noinst_HEADERS = kimageshop_main.h kimageshop_shell.h \
	kimageshop_doc.h kimageshop_view.h \
	canvas.h canvasview.h layer.h layerview.h layerdlg.h \
	brush.h channel.h misc.h tool.h movetool.h brushtool.h kvector.h

METASOURCES = kimageshop_main.moc kimageshop_shell.moc \
	kimageshop_doc.moc kimageshop_view.moc \
	canvasview.moc canvas.moc layer.moc layerview.moc layerdlg.moc

idl_DATA = kimageshop.idl

kdelnk_DATA = kimageshop.desktop
kdelnkdir = $(kde_appsdir)/KOffice

####### Implicit rules

####### Build rules

IDL_FLAGS = --query-server-for-narrow --relative-paths -I$(top_srcdir)/lib/kofficecore -I$(top_srcdir)/lib/store -I$(interfacedir) -I$(idldir)

messages:
	(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot *.cc && mv messages.po ../po/kimageshop.pot

