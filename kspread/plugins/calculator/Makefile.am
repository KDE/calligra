INCLUDES  = -I$(srcdir)/../../../kspread $(KOFFICE_INCLUDES) $(all_includes) \
-I$(top_srcdir)/lib/kscript -I$(top_builddir)/lib/kscript

libkspreadcalc_la_LDFLAGS = -avoid-version -module

libkspreadcalc_la_LIBADD = $(all_libraries) ../../libkspread.la

####### Files

lib_LTLIBRARIES = libkspreadcalc.la

libkspreadcalc_la_SOURCES = configdlg.cpp \
			dlabel.cpp \
			fontdlg.cpp \
			kcalc.cpp \
			kcalc_core.cpp \
			main.cpp \
			stats.cpp

noinst_HEADERS = configdlg.h \
		 dlabel.h \
		 fontdlg.h \
		 kcalc.h \
		 kcalctype.h \
		 main.h \
		 stats.h \
		 version.h

libkspreadcalc_la_METASOURCES = configdlg.moc \
			    dlabel.moc \
			    fontdlg.moc \
			    kcalc.moc \
			    main.moc

kdelnk_DATA = kspreadcalc.desktop
kdelnkdir = $(kde_appsdir)/KOffice

SUBDIRS = pics

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot *.cc && mv messages.po ../po/kspreadcalc_calc.pot


