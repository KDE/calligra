SUBDIRS   = pics

INCLUDES  = $(MICO_INCLUDES) $(KOFFICE_INCLUDES) $(all_includes) -I$(idldir)
LDFLAGS   = $(all_libraries) $(MICO_LDFLAGS) $(KDE_RPATH)
LDADD     = $(LIB_KOFFICEUI) $(LIB_KIMGIO)

####### Files

bin_PROGRAMS = kimage

kimage_SOURCES = \
 kimage.cc \
 kimage_main.cc \
 kimage_shell.cc \
 kimage_doc.cc \
 kimage_view.cc \
 formats.cc \
 preferencesdlg.cc \
 zoomfactordlg.cc \
 kintegerinputdialog.cc \
 kstartparams.cc \
 koShell.cc

noinst_HEADERS = \
 kimage.h \
 kimage_main.h \
 kimage_shell.h \
 kimage_doc.h \
 kimage_view.h \
 formats.h \
 preferencesdlg.h \
 zoomfactordlg.h \
 kintegerinputdialog.h \
 kstartparams.h \
 koShell.h

METASOURCES = \
 kimage_main.moc \
 kimage_shell.moc \
 kimage_doc.moc \
 kimage_view.moc \
 preferencesdlg.moc \
 zoomfactordlg.moc \
 kintegerinputdialog.moc \
 koShell.moc

idl_DATA = kimage.idl
idl_HEADERS = kimage.h

DISTCLEANFILES = $(idl_DATA:.idl=.cc) $(idl_HEADERS) $(METASOURCES)

kdelnk_DATA = kimage.desktop
kdelnkdir = $(kde_appsdir)/KOffice

####### Implicit rules

####### Build rules

IDL_FLAGS = --query-server-for-narrow --relative-paths -I$(top_srcdir)/lib/kofficecore -I$(top_srcdir)/lib/store -I$(interfacedir) -I$(idldir)

messages:
	(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot *.cc && mv messages.po ../po/kimage.pot

kimage.h: kimage.idl $(micodir)/include/CORBA.h
	$(IDL) $(IDL_FLAGS) $(srcdir)/kimage.idl

kimage.cc: kimage.h
kimage_doc.cc: kimage.h
kimage_main.cc: kimage.h
kimage_shell.cc: kimage.h
kimage_view.cc: kimage.h
formats.cc: formats.h
preferencesdlg.cc: preferencesdlg.h
zoomfactordlg.cc: zoomfactordlg.h
kintegerinputdialog.cc: kintegerinputdialog.h
formats.cc: formats.h
kstartparams.cc: kstartparams.h
koShell.cc: koShell.h
