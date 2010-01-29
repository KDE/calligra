

include_directories( ${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/kexi/kexidb ${KDE4_INCLUDES}  )

ADD_DEFINITIONS( -D__KEXIDB__= )

########### next target ###############

set(kexidb_sqlite3driver_PART_SRCS 
   sqliteconnection.cpp 
   sqlitedriver.cpp 
   sqlitecursor.cpp 
   sqlitekeywords.cpp 
   sqlitepreparedstatement.cpp
   sqliteadmin.cpp
   sqlitevacuum.cpp
   sqlitealter.cpp
)


kde4_add_plugin(kexidb_sqlite3driver ${kexidb_sqlite3driver_PART_SRCS})



target_link_libraries(kexidb_sqlite3driver ${QT_QT3SUPPORT_LIBRARY} ${KDE4_KPARTS_LIBS} sqlite3 kexidb kexiutils )

install(TARGETS kexidb_sqlite3driver  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kexidb_sqlite3driver.desktop  DESTINATION ${SERVICES_INSTALL_DIR})





#original Makefile.am contents follow:

#include $(top_srcdir)/kexi/Makefile.global
#
#kde_module_LTLIBRARIES = kexidb_sqlite3driver.la
#
#INCLUDES = -I$(top_srcdir)/kexi/3rdparty/kexisql3/src -I$(srcdir)/../..  \
#	-I$(top_srcdir)/kexi $(all_includes)
#
#kexidb_sqlite3driver_la_METASOURCES = AUTO
#
#kexidb_sqlite3driver_la_SOURCES = sqliteconnection.cpp sqlitedriver.cpp sqlitecursor.cpp \
#sqlitekeywords.cpp sqlitepreparedstatement.cpp
#
#kexidb_sqlite3driver_la_LIBADD = $(LIB_KPARTS) $(LIB_QT) \
# $(top_builddir)/kexi/3rdparty/kexisql3/src/libkexisql3.la \
# $(top_builddir)/kexi/kexidb/libkexidb.la \
# $(top_builddir)/kexi/kexidb/parser/libkexidbparser.la
#
#kexidb_sqlite3driver_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN) $(VER_INFO) 
#
#
#kde_services_DATA = kexidb_sqlite3driver.desktop
#
#
#KDE_CXXFLAGS += -DKEXIDB_SQLITE_DRIVER_EXPORT= -D__KEXIDB__= \
#	-include $(top_srcdir)/kexi/kexidb/global.h
#
