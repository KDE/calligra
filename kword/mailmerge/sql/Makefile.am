AM_CPPFLAGS = $(KOFFICE_INCLUDES) -I$(top_srcdir)/lib/kformula \
	      $(KOTEXT_INCLUDES) -I$(top_srcdir)/kword $(all_includes)

kde_module_LTLIBRARIES = kwmailmerge_qtsqldb.la kwmailmerge_qtsqldb_power.la

## Plugin encapsulating the QT SQL database interface
kwmailmerge_qtsqldb_la_SOURCES = KWQtSqlSerialDataSource.cpp qtsqldatasourceeditor.ui \
				 KWQtSqlEasyFilter.cpp
kwmailmerge_qtsqldb_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
kwmailmerge_qtsqldb_la_LIBADD = libmailmergesqlcommon.la ../../libkwmailmerge_interface.la $(LIB_KDEUI)
kwmailmerge_qtsqldb_la_COMPILE_FIRST = qtsqlopenwidget.h

## 2. Plugin encapsulating the QT SQL database interface
kwmailmerge_qtsqldb_power_la_SOURCES = KWQtSqlPowerSerialDataSource.cpp kwqtsqlpower.ui KWQtSqlPowerSerialDataSource.skel
kwmailmerge_qtsqldb_power_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
kwmailmerge_qtsqldb_power_la_LIBADD = libmailmergesqlcommon.la ../../libkwmailmerge_interface.la $(LIB_KDEUI)
kwmailmerge_qtsqldb_power_la_COMPILE_FIRST = qtsqlopenwidget.h

# Common files
noinst_LTLIBRARIES = libmailmergesqlcommon.la
libmailmergesqlcommon_la_SOURCES = KWQtSqlMailMergeOpen.cpp KWQtSqlSerialDataSourceBase.skel qtsqlopenwidget.ui KWQtSqlSerialDataSourceBase.cpp

METASOURCES = AUTO

#services_DATA=kwserialletter_qtsqldb.desktop kwserialletter_qtsqldb_power.desktop
services_DATA=kwserialletter_qtsqldb_power.desktop
servicesdir=$(kde_servicesdir)

