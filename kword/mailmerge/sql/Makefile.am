
KDE_CXXFLAGS = $(USE_RTTI)
INCLUDES = $(KOFFICE_INCLUDES) -I$(top_srcdir)/lib/kformula \
	$(KOTEXT_INCLUDES) -I$(top_srcdir)/kword $(all_includes)

kde_module_LTLIBRARIES = kwmailmerge_qtsqldb.la kwmailmerge_qtsqldb_power.la

## Plugin encapsulating the QT SQL database interface
kwmailmerge_qtsqldb_la_SOURCES = serialletter_qtsqlplugin.cc qtsqldatasourceeditor.ui serialletter_qtsql_base.cc qtsqlopenwidget.ui qtsqlopeneditor.cc serialletter_qtsql_base.skel
kwmailmerge_qtsqldb_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
kwmailmerge_qtsqldb_la_LIBADD = ../../libkwmailmerge_interface.la $(LIB_KDEUI) -lDCOP

## 2. Plugin encapsulating the QT SQL database interface
kwmailmerge_qtsqldb_power_la_SOURCES = serialletter_qtsql_power_plugin.cc qtsqlopenwidget.ui kwqtsqlpower.ui serialletter_qtsql_base.cc qtsqlopeneditor.cc serialletter_qtsql_base.skel serialletter_qtsql_power_plugin.skel
kwmailmerge_qtsqldb_power_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
kwmailmerge_qtsqldb_power_la_LIBADD = ../../libkwmailmerge_interface.la $(LIB_KDEUI) -lDCOP

METASOURCES = AUTO

services_DATA=kwserialletter_qtsqldb.desktop kwserialletter_qtsqldb_power.desktop
servicesdir=$(kde_servicesdir)

SUBDIRS = .
