####### General

SUBDIRS   = pics

INCLUDES  = $(KOFFICE_INCLUDES) $(all_includes) 

kimage_LDFLAGS   = $(all_libraries) $(KDE_RPATH)
#kimage_LDFLAGS   = $(all_libraries) $(KDE_RPATH) -fno-rtti
#kimage_LDADD     = $(LIB_KOFFICEUI)
#kimage_LDADD     = ../lib/kofficeui/libkofficeui.la
kimage_LDADD     = $(LIB_KOFFICEUI) $(LIB_KIMGIO)

#libkimage_la_LDFLAGS = $(all_libraries) -avoid-version -module 
#libkimage_la_LIBADD  = $(LIB_KIMGIO)

####### Files

bin_PROGRAMS = kimage

#kimage_SOURCES = main.cc
kimage_SOURCES = main.cc kimage_shell.cc kimage_factory.cc kimage_doc.cc \
	kimage_view.cc preferencesdlg.cc zoomfactordlg.cc kintegerinputdialog.cc
kimage_METASOURCES = kimage_main.moc kimage_shell.moc kimage_factory.moc kimage_doc.moc \
	kimage_view.moc preferencesdlg.moc zoomfactordlg.moc kintegerinputdialog.moc

#lib_LTLIBRARIES = libkimage.la

#libkimage_la_SOURCES = kimage_shell.cc kimage_factory.cc kimage_doc.cc \
#	kimage_view.cc preferencesdlg.cc zoomfactordlg.cc kintegerinputdialog.cc

noinst_HEADERS = kimage_shell.h kimage_factory.h kimage_doc.h \
	kimage_view.h preferencesdlg.h zoomfactordlg.h kintegerinputdialog.h

#libkimage_la_METASOURCES = kimage_shell.moc kimage_factory.moc kimage_doc.moc \
#	kimage_view.moc preferencesdlg.moc zoomfactordlg.moc kintegerinputdialog.moc

kdelnk_DATA = kimage.desktop
kdelnkdir = $(kde_appsdir)/KOffice

kdelnk_DATA = kimage.desktop
kdelnkdir = $(kde_appsdir)/KOffice

kdemime_DATA = x-kimage.desktop
kdemimedir = $(kde_mimedir)/application

rcdir = $(kde_datadir)/kimage
rc_DATA = kimage.rc kimage_shell.rc

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot *.cc && mv messages.po ../po/kimage.pot

