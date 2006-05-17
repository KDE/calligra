noinst_LTLIBRARIES = libkivioconfig.la
INCLUDES = -I$(top_srcdir)/kivio/kiviopart -I.. \
	-I$(top_srcdir)/kivio/kiviopart/kiviosdk \
	$(KOFFICE_INCLUDES) \
	$(KOTEXT_INCLUDES) \
	$(all_includes)

libkivioconfig_la_METASOURCES = AUTO

libkivioconfig_la_SOURCES = kivio_settings.kcfgc kivio_config.cpp
noinst_HEADERS = kivio_config.h kivio_settings.h

kivio_config.lo: kivio_settings.h

kde_kcfg_DATA = kivio.kcfg
