INCLUDES  = -I$(srcdir)/widgets -I$(srcdir)/dialogs -I$(srcdir)/tools \
			 $(MICO_INCLUDES) $(KOFFICE_INCLUDES) $(all_includes) 
LDFLAGS   = $(all_libraries) $(MICO_LDFLAGS) $(KDE_RPATH)
LDADD     = $(LIB_KOFFICEUI) $(LIB_KIMGIO) widgets/libkiswidgets.la dialogs/libkisdialogs.la tools/libkistools.la

SUBDIRS   = widgets dialogs tools . pics data

####### Files

bin_PROGRAMS = kimageshop 

#canvastest
#canvastest_SOURCES = canvastest.cc channeldata.cc layer.cc canvas.cc misc.cc

kimageshop_SOURCES = channeldata.cc layer.cc canvas.cc \
	brush.cc misc.cc kimageshop_undo.cc kimageshop_config.cc kvector.cc \
	canvasview.cc tool.cc kfloatingdialog.cc kfloatingtabdialog.cc kcolor.cc \
	kfloatingdialogowner.cc kimageshop_shell.cc kimageshop_doc.cc \
	kimageshop_view.cc kimageshop_main.cc  

noinst_HEADERS = kimageshop_main.h kimageshop_shell.h \
	kimageshop_doc.h kimageshop_view.h kimageshop_undo.h kimageshop_config.h \
	kvector.h kfloatingdialog.h kcolor.h misc.h tool.h \
	canvas.h canvasview.h layer.h brush.h channel.h \
	kfloatingtabdialog.h kfloatingdialogowner.h

METASOURCES = kimageshop_main.moc kimageshop_shell.moc \
	kimageshop_doc.moc kimageshop_view.moc kimageshop_config.moc \
	canvasview.moc canvas.moc layer.moc kfloatingdialog.moc \
	kfloatingtabdialog.moc

idl_DATA = kimageshop.idl

kdelnk_DATA = kimageshop.desktop
kdelnkdir = $(kde_appsdir)/KOffice

####### Build rules

#IDL_FLAGS = --query-server-for-narrow --relative-paths -I$(top_srcdir)/lib/kofficecore -I$(top_srcdir)/lib/store -I$(interfacedir) -I$(idldir)

messages:
	$(XGETTEXT) -C  -ktranslate -kaliasLocal -ki18n -x $(includedir)/kde.pot *.h *.cc *.cpp && mv messages.po ../po/kimageshop.pot
	for subdir in $(SUBDIRS); do \
	   if test ! "x$$subdir" = "x."; then \
	   echo "Making messages in $$subdir"; \
	   (cd $$subdir && $(MAKE) -f Makefile.am messages) || exit 1; \
	 fi ;\
	done
