SUBDIRS   = pics

INCLUDES  = $(MICO_INCLUDES) $(KOFFICE_INCLUDES) $(all_includes) 
LDFLAGS   = $(all_libraries) $(MICO_LDFLAGS) $(KDE_RPATH)
LDADD     = $(LIB_KOFFICEUI) $(LIB_KIMGIO)

####### Files

bin_PROGRAMS = kimage

kimage_SOURCES = \
 kimage.idl \
 kimage_main.cc \
 kimage_shell.cc \
 kimage_doc.cc \
 kimage_view.cc \
 preferencesdlg.cc \
 zoomfactordlg.cc \
 kintegerinputdialog.cc

noinst_HEADERS = \
 kimage_main.h \
 kimage_shell.h \
 kimage_doc.h \
 kimage_view.h \
 preferencesdlg.h \
 zoomfactordlg.h \
 kintegerinputdialog.h

METASOURCES = \
 kimage_main.moc \
 kimage_shell.moc \
 kimage_doc.moc \
 kimage_view.moc \
 preferencesdlg.moc \
 zoomfactordlg.moc \
 kintegerinputdialog.moc

idl_DATA = kimage.idl

kdelnk_DATA = kimage.desktop
kdelnkdir = $(kde_appsdir)/KOffice

IDL_FLAGS = --relative-paths -I$(top_srcdir)/lib/kofficecore -I$(top_srcdir)/lib/store -I$(interfacedir) -I$(idldir)

messages:
	(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot *.cc && mv messages.po ../po/kimage.pot

