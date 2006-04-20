kde4_header()

include_directories( ${CMAKE_SOURCE_DIR}/kexi ${KDE4_INCLUDES}  )


########### next target ###############

set(keximigrate_mysql_PART_SRCS mysqlmigrate.cpp )

kde4_automoc(${keximigrate_mysql_PART_SRCS})

kde4_add_plugin(keximigrate_mysql ${keximigrate_mysql_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} keximigrate_mysql )

target_link_libraries(keximigrate_mysql  ${KDE4_KDECORE_LIBS} ${KDE4_KPARTS_LIBS} keximigrate mysqlclient )

install_targets(${PLUGIN_INSTALL_DIR} keximigrate_mysql )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES  keximigrate_mysql.desktop )

kde4_footer()



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
