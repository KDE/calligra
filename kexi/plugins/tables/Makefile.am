include $(top_srcdir)/kexi/Makefile.global

kde_module_LTLIBRARIES = kexihandler_table.la

kexihandler_table_la_SOURCES = kexitablepart.cpp kexialtertabledialog.cpp kexialtertable_dataview.cpp \
	kexitabledesignercommands.cpp

kexihandler_table_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) $(VER_INFO) -module
kexihandler_table_la_LIBADD = $(top_builddir)/kexi/core/libkexicore.la \
	$(top_builddir)/kexi/kexidb/libkexidb.la \
	$(top_builddir)/kexi/widget/tableview/libkexidatatable.la \
	$(top_builddir)/kexi/widget/libkexiextendedwidgets.la \
	$(top_builddir)/lib/koproperty/libkoproperty.la

INCLUDES= $(KOFFICE_INCLUDES) \
	-I$(top_srcdir)/kexi/core -I$(top_srcdir)/kexi \
	-I$(top_srcdir)/kexi/widget -I$(top_srcdir)/kexi/widget/tableview \
	-I$(top_srcdir)/kexi/kexidb -I$(top_srcdir)/lib  $(all_includes)

servicesdir=$(kde_servicesdir)/kexi
services_DATA=kexitablehandler.desktop

rcdir = $(kde_datadir)/kexi
rc_DATA = kexitablepartui.rc kexitablepartinstui.rc

METASOURCES = AUTO

include ../Makefile.common
