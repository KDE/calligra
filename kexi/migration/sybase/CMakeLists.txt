include_directories( ${FreeTDS_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/kexi ${KDE4_INCLUDES}  )

########### next target ###############

set(keximigrate_sybase_PART_SRCS sybasemigrate.cpp )

kde4_add_plugin(keximigrate_sybase ${keximigrate_sybase_PART_SRCS})



target_link_libraries(keximigrate_sybase ${FreeTDS_LIBRARIES} ${KDE4_KDECORE_LIBS}
 ${KDE4_KPARTS_LIBS} kexiutils kexidb keximigrate ${CALLIGRADB_LIBS})

install(TARGETS keximigrate_sybase  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  keximigrate_sybase.desktop  DESTINATION ${SERVICES_INSTALL_DIR})





#original Makefile.am contents follow:

#include $(top_srcdir)/kexi/Makefile.global
#
#kde_module_LTLIBRARIES = keximigrate_mysql.la
#
#INCLUDES = -I$(srcdir)/../../.. -I$(top_srcdir)/kexi $(all_includes) -I$(MYSQL_INC)
#
#keximigrate_mysql_la_METASOURCES = AUTO
#
#keximigrate_mysql_la_SOURCES = mysqlmigrate.cpp
#
#keximigrate_mysql_la_LIBADD = $(LIB_KPARTS) $(LIB_QT) ../libkeximigrate.la $(MYSQL_LIBS) -lmysqlclient
#
#keximigrate_mysql_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN) $(VER_INFO) -no-undefined
#
#kde_services_DATA = keximigrate_mysql.desktop
#
#noinst_HEADERS =  mysqlmigrate.h
#
