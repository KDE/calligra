include $(top_srcdir)/kexi/Makefile.global

kde_module_LTLIBRARIES = kexikspreadimport.la

kexikspreadimport_la_SOURCES = kexikspreadsourcebase.ui kexikspreadsource.cpp kexikspreadimport.cpp
kexikspreadimport_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) -module
kexikspreadimport_la_LIBADD = ../../../core/libkexicore.la

INCLUDES= $(KOFFICE_INCLUDES) -I$(top_srcdir)/kexi/core -I$(top_srcdir)/kexi \
		-I$(top_srcdir)/kexi/widget -I$(top_srcdir)/kexi/tableview \
		-I$(top_srcdir)/kexi/kexiDB -I$(top_srcdir)/kexi/plugins/tables \
		$(all_includes)

METASOURCES = AUTO

servicesdir=$(kde_servicesdir)/kexi
services_DATA = kexikspreadimport.desktop

