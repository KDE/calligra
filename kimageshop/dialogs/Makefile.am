INCLUDES = -I$(srcdir)/.. -I$(srcdir)/../widgets $(KOFFICE_INCLUDES) $(all_includes) 

IDL_FLAGS = --relative-paths -I$(top_srcdir)/lib/kofficecore -I$(top_srcdir)/lib/store -I$(interfacedir) -I$(idldir)

noinst_LTLIBRARIES = libkisdialogs.la

libkisdialogs_la_SOURCES = gradientdlg.cc brushdialog.cc brusheswidget.cc \
	colordialog.cc gradienteditordlg.cc gradientview.cc \
	newdialog.cc preferencesdlg.cc layerview.cc channelview.cc layerdlg.cc \
	kfloatingdialog.cc kfloatingtabdialog.cc kfloatingdialogowner.cc

libkisdialogs_la_METASOURCES = brushdialog.moc brusheswidget.moc \
	colordialog.moc gradientdlg.moc gradienteditordlg.moc gradientview.moc \
	newdialog.moc preferencesdlg.moc layerview.moc channelview.moc layerdlg.moc \
	kfloatingdialog.moc kfloatingtabdialog.moc

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot *.cc *.h && mv messages.po ../../po/libkisdialogs.pot
