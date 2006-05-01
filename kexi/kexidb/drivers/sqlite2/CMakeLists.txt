kde4_header()

include_directories( ${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/kexi/kexidb ${CMAKE_SOURCE_DIR}/kexi/3rdparty/kexisql/src ${CMAKE_BUILD_DIR}/kexi/kexidb/drivers/sqlite
${KDE4_INCLUDES}  )

ADD_DEFINITIONS( -D__KEXIDB__= -DSQLITE2= -include ${CMAKE_SOURCE_DIR}/kexi/kexidb/global.h )

########### next target ###############

set(kexidb_sqlite2driver_PART_SRCS 
   ../sqlite/sqliteconnection.cpp 
   ../sqlite/sqlitedriver.cpp 
   ../sqlite/sqlitecursor.cpp 
   ../sqlite/sqlitekeywords.cpp 
   ../sqlite/sqlitepreparedstatement.cpp
   ../sqlite/sqliteadmin.cpp )

kde4_automoc(${kexidb_sqlite2driver_PART_SRCS})

kde4_add_plugin(kexidb_sqlite2driver ${kexidb_sqlite2driver_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kexidb_sqlite2driver )

target_link_libraries(kexidb_sqlite2driver  ${KDE4_KDECORE_LIBS} ${KDE4_KPARTS_LIBS} kexisql2 kexidb )

install_targets(${PLUGIN_INSTALL_DIR} kexidb_sqlite2driver )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES  kexidb_sqlite2driver.desktop )

kde4_footer()



#original Makefile.am contents follow:

#include $(top_srcdir)/kexi/Makefile.global
#
#kde_module_LTLIBRARIES = kexidb_sqlite2driver.la
#
#INCLUDES = -I$(top_srcdir)/kexi \
#	-I$(top_srcdir)/kexi/kexidb \
#	-I$(top_srcdir)/kexi/3rdparty/kexisql/src \
#	-I$(srcdir)/../sqlite \
#	-I$(srcdir)/../sqlite/moc \
#	-I../sqlite \
#	$(all_includes)
#
#kexidb_sqlite2driver_la_METASOURCES = AUTO
#
#kexidb_sqlite2driver_la_SOURCES = sqliteconnection.cpp sqlitedriver.cpp sqlitecursor.cpp \
#	sqlitepreparedstatement.cpp
#
#kexidb_sqlite2driver_la_LIBADD = $(LIB_KPARTS) $(LIB_QT) \
# $(top_builddir)/kexi/3rdparty/kexisql/src/libkexisql2.la \
# $(top_builddir)/kexi/kexidb/libkexidb.la \
# $(top_builddir)/kexi/kexidb/parser/libkexidbparser.la
#
#kexidb_sqlite2driver_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN) $(VER_INFO) 
#
#
#kde_services_DATA = kexidb_sqlite2driver.desktop
#
#
#KDE_CXXFLAGS += -DKEXIDB_SQLITE_DRIVER_EXPORT= -D__KEXIDB__= \
#	-DSQLITE2= -include $(top_srcdir)/kexi/kexidb/global.h
#
