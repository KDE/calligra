INCLUDES = -I.. -I../widgets $(all_includes) $(KOFFICE_INCLUDES)

IDL_FLAGS = --query-server-for-narrow --relative-paths -I$(top_srcdir)/lib/kofficecore -I$(top_srcdir)/lib/store -I$(interfacedir) -I$(idldir)
lib_LTLIBRARIES = libkisdialogs.la
libkisdialogs_la_LDFLAGS = $(all_libraries) -version-info 1:0
#libkisdialogs_la_LIBADD =

libkisdialogs_la_SOURCES = kimageshop.idl brushdialog.cc brusheswidget.cc channelview.cc \
	colordialog.cc gradientdlg.cc gradienteditordlg.cc gradientview.cc \
	layerdlg.cc layerview.cc newdialog.cc preferencesdlg.cc

libkisdialogs_la_METASOURCES = brushdialog.moc brusheswidget.moc channelview.moc \
	colordialog.moc gradientdlg.moc gradienteditordlg.moc gradientview.moc \
	layerdlg.moc layerview.moc newdialog.moc preferencesdlg.moc

#include_HEADERS =

kimageshop_IDLDIR = ..

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot *.cc *.h && mv messages.po ../../po/libkisdialogs.pot
