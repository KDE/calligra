INCLUDES  = -I$(srcdir)/../../../kspread $(KOFFICE_INCLUDES) $(KOTEXT_INCLUDES) -I$(top_srcdir)/lib/koscript -I$(top_builddir)/lib/koscript $(all_includes)

kde_module_LTLIBRARIES = libkspreadinsertcalendar.la

libkspreadinsertcalendar_la_SOURCES = kspread_plugininsertcalendar.cc \
				kspread_insertcalendardialogbase.ui \
				kspread_insertcalendardialog.cc
libkspreadinsertcalendar_la_LDFLAGS = $(all_libraries) -avoid-version -module -no-undefined
libkspreadinsertcalendar_la_LIBADD = ../../libkspreadcommon.la

METASOURCES = AUTO

# Delete old unused file
#install-data-local: uninstall.desktop
#	$(mkinstalldirs) $(DESTDIR)$(kde_servicesdir)
#	$(INSTALL_DATA) $(srcdir)/uninstall.desktop $(DESTDIR)$(kde_servicesdir)/kspreadinsertcalendar.desktop

rc_DATA = insertcalendar.rc
rcdir = $(kde_datadir)/kspread/kpartplugins



SUBDIRS =

messages: rc.cpp
	$(XGETTEXT) rc.cpp *.cc -o $(podir)/kspreadinsertcalendar.pot
