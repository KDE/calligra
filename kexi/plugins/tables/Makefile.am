kde_module_LTLIBRARIES = kexihandler_table.la

kexihandler_table_la_SOURCES = kexitablepart.cpp kexitablepartproxy.cpp kexialtertable.cpp
kexihandler_table_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) -module
kexihandler_table_la_LIBADD = ../../core/libkexicore.la ../../tableview/libkexidatatable.la \
		../../widget/libkexiextendedwidgets.la ../../kexiDB/libkexidb.la ../../formeditor/libkformeditor.la

INCLUDES= $(KOFFICE_INCLUDES) -I$(top_srcdir)/kexi/core -I$(top_srcdir)/kexi \
		-I$(top_srcdir)/kexi/widget -I$(top_srcdir)/kexi/tableview \
		-I$(top_srcdir)/kexi/kexiDB $(all_includes)

METASOURCES = AUTO

include $(top_srcdir)/kexi/Makefile.global
