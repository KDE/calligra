INCLUDES  = -I$(srcdir)/../../../kspread $(KOFFICE_INCLUDES) $(all_includes) \
-I$(top_srcdir)/lib/koscript -I$(top_builddir)/lib/koscript

kde_module_LTLIBRARIES = libkspreadcalc.la

libkspreadcalc_la_SOURCES = configdlg.cpp \
			dlabel.cpp \
			fontdlg.cpp \
			kcalc.cpp \
			kcalc_core.cpp \
			main.cpp \
			stats.cpp
libkspreadcalc_la_LDFLAGS = -avoid-version -module -no-undefined
libkspreadcalc_la_LIBADD = $(all_libraries) ../../libkspreadpart.la

METASOURCES = AUTO

desktop_DATA = kspreadcalc.desktop
desktopdir = $(kde_servicesdir)

rc_DATA = kspreadcalc.rc
rcdir = $(kde_datadir)/kspread/kpartplugins

SUBDIRS = pics

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/kspreadcalc_calc.pot


