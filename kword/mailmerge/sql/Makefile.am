AM_CPPFLAGS = $(KOFFICE_INCLUDES) -I$(top_srcdir)/lib/kformula \
	      $(KOTEXT_INCLUDES) -I$(top_srcdir)/kword $(all_includes)

kde_module_LTLIBRARIES = kwmailmerge_qtsqldb.la kwmailmerge_qtsqldb_power.la

## Plugin encapsulating the QT SQL database interface
kwmailmerge_qtsqldb_la_SOURCES = serialletter_qtsqlplugin.cc qtsqldatasourceeditor.ui \
				 mailmerge_qtsqlplugin_easyfilter.cc
kwmailmerge_qtsqldb_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
kwmailmerge_qtsqldb_la_LIBADD = libmailmergesqlcommon.la ../../libkwmailmerge_interface.la $(LIB_KDEUI)

## 2. Plugin encapsulating the QT SQL database interface
kwmailmerge_qtsqldb_power_la_SOURCES = serialletter_qtsql_power_plugin.cc kwqtsqlpower.ui serialletter_qtsql_power_plugin.skel
kwmailmerge_qtsqldb_power_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
kwmailmerge_qtsqldb_power_la_LIBADD = libmailmergesqlcommon.la ../../libkwmailmerge_interface.la $(LIB_KDEUI)

# Common files
noinst_LTLIBRARIES = libmailmergesqlcommon.la
libmailmergesqlcommon_la_SOURCES = qtsqlopeneditor.cc serialletter_qtsql_base.skel qtsqlopenwidget.ui serialletter_qtsql_base.cc

METASOURCES = AUTO

#services_DATA=kwserialletter_qtsqldb.desktop kwserialletter_qtsqldb_power.desktop
services_DATA=kwserialletter_qtsqldb_power.desktop
servicesdir=$(kde_servicesdir)

