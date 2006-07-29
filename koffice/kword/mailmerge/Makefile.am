
KDE_CXXFLAGS = $(USE_RTTI)
INCLUDES = $(KOFFICE_INCLUDES) -I$(top_srcdir)/lib/kformula $(KOTEXT_INCLUDES) -I$(top_srcdir)/kword $(all_includes)

kde_module_LTLIBRARIES = kwmailmerge_classic.la

## Plugin encapsulating the (old) internally stored style
kwmailmerge_classic_la_SOURCES = KWClassicSerialDataSource.cpp
kwmailmerge_classic_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
kwmailmerge_classic_la_LIBADD = ../libkwmailmerge_interface.la $(LIB_KDEUI) $(LIB_KOTEXT)

METASOURCES = AUTO

services_DATA=kwserialletter_classic.desktop
servicesdir=$(kde_servicesdir)

if include_sql
SQLDIRECTORY=sql
endif

SUBDIRS = . kabc kspread $(SQLDIRECTORY)

