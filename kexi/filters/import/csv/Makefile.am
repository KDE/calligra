kde_module_LTLIBRARIES = kexicsvimport.la

kexicsvimport_la_SOURCES = preview.ui kexicsvimport.cpp
kexicsvimport_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) -module
kexicsvimport_la_LIBADD = ../../../core/libkexicore.la

KDE_CXXFLAGS = $(USE_EXCEPTIONS)

INCLUDES= $(KOFFICE_INCLUDES) -I$(top_srcdir)/kexi/core -I$(top_srcdir)/kexi \
		-I$(top_srcdir)/kexi/widget -I$(top_srcdir)/kexi/tableview \
		-I$(top_srcdir)/kexi/kexiDB -I$(top_srcdir)/kexi/plugins/tables $(all_includes)

METASOURCES = AUTO

servicesdir=$(kde_servicesdir)/kexi
services_DATA = kexicsvimport.desktop
