include $(top_srcdir)/kexi/Makefile.global

kde_module_LTLIBRARIES = kexihandler_table.la

kexihandler_table_la_SOURCES = kexitablepart.cpp kexialtertabledialog.cpp kexialtertable_dataview.cpp
kexihandler_table_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) $(VER_INFO) -module
kexihandler_table_la_LIBADD = ../../core/libkexicore.la \
  ../../kexidb/libkexidb.la \
  ../../widget/tableview/libkexidatatable.la \
  ../../widget/libkexiextendedwidgets.la \
  ../../widget/propertyeditor/libkexipropertyeditor.la

INCLUDES= $(KOFFICE_INCLUDES) \
	-I$(top_srcdir)/kexi/core -I$(top_srcdir)/kexi \
	-I$(top_srcdir)/kexi/widget -I$(top_srcdir)/kexi/widget/tableview \
	-I$(top_srcdir)/kexi/kexidb -I$(top_srcdir)/kexi/widget/propertyeditor $(all_includes)

METASOURCES = AUTO

include ../Makefile.common
