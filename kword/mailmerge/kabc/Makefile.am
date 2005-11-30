
KDE_CXXFLAGS = $(USE_RTTI)
INCLUDES = $(KOFFICE_INCLUDES) -I$(top_srcdir)/lib/kformula \
	$(KOTEXT_INCLUDES) -I$(top_srcdir)/kword $(all_includes)

kde_module_LTLIBRARIES = kwmailmerge_kabc.la

## Plugin encapsulating the QT SQL database interface
kwmailmerge_kabc_la_SOURCES = KWMailMergeKABC.cpp KWMailMergeKABCConfig.cpp addresspicker.ui
kwmailmerge_kabc_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
kwmailmerge_kabc_la_LIBADD = ../../libkwmailmerge_interface.la $(LIB_KDEUI) $(LIB_KABC)

METASOURCES = AUTO

services_DATA=kwmailmerge_kabc.desktop
servicesdir=$(kde_servicesdir)

