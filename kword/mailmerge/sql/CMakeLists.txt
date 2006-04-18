kde4_header()




########### next target ###############

set(kwmailmerge_qtsqldb_PART_SRCS KWQtSqlSerialDataSource.cpp KWQtSqlEasyFilter.cpp )

kde4_automoc(${kwmailmerge_qtsqldb_PART_SRCS})

kde4_add_ui3_files(kwmailmerge_qtsqldb_PART_SRCS qtsqldatasourceeditor.ui )

kde4_add_plugin(kwmailmerge_qtsqldb ${kwmailmerge_qtsqldb_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kwmailmerge_qtsqldb )

target_link_libraries(kwmailmerge_qtsqldb  ${KDE4_KDECORE_LIBS} mailmergesqlcommon kwmailmerge_interface ${KDE4_KDEUI_LIBS} )

install_targets(${PLUGIN_INSTALL_DIR} kwmailmerge_qtsqldb )


########### next target ###############

set(kwmailmerge_qtsqldb_power_PART_SRCS KWQtSqlPowerSerialDataSource.cpp )

kde4_automoc(${kwmailmerge_qtsqldb_power_PART_SRCS})

kde4_add_ui3_files(kwmailmerge_qtsqldb_power_PART_SRCS kwqtsqlpower.ui )

kde4_add_dcop_skels(kwmailmerge_qtsqldb_power_PART_SRCS KWQtSqlPowerSerialDataSource.h )

kde4_add_plugin(kwmailmerge_qtsqldb_power ${kwmailmerge_qtsqldb_power_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kwmailmerge_qtsqldb_power )

target_link_libraries(kwmailmerge_qtsqldb_power  ${KDE4_KDECORE_LIBS} mailmergesqlcommon kwmailmerge_interface ${KDE4_KDEUI_LIBS} )

install_targets(${PLUGIN_INSTALL_DIR} kwmailmerge_qtsqldb_power )


########### next target ###############


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES  kwserialletter_qtsqldb_power.desktop )

kde4_footer()



#original Makefile.am contents follow:

#AM_CPPFLAGS = $(KOFFICE_INCLUDES) -I$(top_srcdir)/lib/kformula \
#	      $(KOTEXT_INCLUDES) -I$(top_srcdir)/kword $(all_includes)
#
#kde_module_LTLIBRARIES = kwmailmerge_qtsqldb.la kwmailmerge_qtsqldb_power.la
#
### Plugin encapsulating the QT SQL database interface
#kwmailmerge_qtsqldb_la_SOURCES = KWQtSqlSerialDataSource.cpp qtsqldatasourceeditor.ui \
#				 KWQtSqlEasyFilter.cpp
#kwmailmerge_qtsqldb_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
#kwmailmerge_qtsqldb_la_LIBADD = libmailmergesqlcommon.la ../../libkwmailmerge_interface.la $(LIB_KDEUI)
#kwmailmerge_qtsqldb_la_COMPILE_FIRST = qtsqlopenwidget.h
#
### 2. Plugin encapsulating the QT SQL database interface
#kwmailmerge_qtsqldb_power_la_SOURCES = KWQtSqlPowerSerialDataSource.cpp kwqtsqlpower.ui KWQtSqlPowerSerialDataSource.skel
#kwmailmerge_qtsqldb_power_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
#kwmailmerge_qtsqldb_power_la_LIBADD = libmailmergesqlcommon.la ../../libkwmailmerge_interface.la $(LIB_KDEUI)
#kwmailmerge_qtsqldb_power_la_COMPILE_FIRST = qtsqlopenwidget.h
#
## Common files
#noinst_LTLIBRARIES = libmailmergesqlcommon.la
#libmailmergesqlcommon_la_SOURCES = KWQtSqlMailMergeOpen.cpp KWQtSqlSerialDataSourceBase.skel qtsqlopenwidget.ui KWQtSqlSerialDataSourceBase.cpp
#
#METASOURCES = AUTO
#
##services_DATA=kwserialletter_qtsqldb.desktop kwserialletter_qtsqldb_power.desktop
#services_DATA=kwserialletter_qtsqldb_power.desktop
#servicesdir=$(kde_servicesdir)
#
