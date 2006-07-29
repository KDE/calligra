INCLUDES  = -I$(srcdir)/../../../kspread $(KOFFICE_INCLUDES) $(KOTEXT_INCLUDES) -I$(top_srcdir)/lib/koscript -I$(top_builddir)/lib/koscript $(all_includes)

kde_module_LTLIBRARIES = libkspreadcalc.la

libkspreadcalc_la_SOURCES = configdlg.cpp \
			dlabel.cpp \
			kcalc.cpp \
			kcalc_core.cpp \
			main.cpp \
			stats.cpp
libkspreadcalc_la_LDFLAGS = $(all_libraries) -avoid-version -module -no-undefined
libkspreadcalc_la_LIBADD = ../../libkspreadcommon.la

METASOURCES = AUTO

# Delete old unused file
install-data-local: uninstall.desktop
	$(mkinstalldirs) $(DESTDIR)$(kde_servicesdir)
	$(INSTALL_DATA) $(srcdir)/uninstall.desktop $(DESTDIR)$(kde_servicesdir)/kspreadcalc.desktop

rc_DATA = kspreadcalc.rc
rcdir = $(kde_datadir)/kspread/kpartplugins

SUBDIRS = pics

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/kspreadcalc_calc.pot


