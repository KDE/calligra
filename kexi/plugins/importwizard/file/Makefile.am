include $(top_srcdir)/kexi/Makefile.global

kde_module_LTLIBRARIES = kexifileimport.la

kexifileimport_la_SOURCES =  kexifileimportwizard.cpp kexiimportfiledialogwidget.cpp
kexifileimport_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) -module
kexifileimport_la_LIBADD = ../../../core/libkexicore.la ../common/libkexiimportwizard.la


KDE_CXXFLAGS += $(USE_EXCEPTIONS) -DKEXIFILTER_EXPORT= -DKEXIDB_EXPORT= -DKEXICORE_EXPORT=

INCLUDES= $(KOFFICE_INCLUDES) -I$(top_srcdir)/kexi/core -I$(top_srcdir)/kexi \
		-I$(top_srcdir)/kexi/widget -I$(top_srcdir)/kexi/tableview \
		-I$(top_srcdir)/kexi/kexiDB -I$(top_srcdir)/kexi/plugins/tables \
		-I$(top_srcdir)/kexi/plugins/importwizard/common $(all_includes)

METASOURCES = AUTO
