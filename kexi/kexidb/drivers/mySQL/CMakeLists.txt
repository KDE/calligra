kde4_header()

include_directories( ${CMAKE_SOURCE_DIR}/kexi ${KDE4_INCLUDES}  )


########### next target ###############

set(kexidb_mysqldriver_PART_SRCS 
   mysqldriver.cpp 
   mysqlconnection.cpp 
   mysqlconnection_p.cpp 
   mysqlcursor.cpp 
   mysqlkeywords.cpp 
   mysqlpreparedstatement.cpp )

kde4_automoc(${kexidb_mysqldriver_PART_SRCS})

kde4_add_plugin(kexidb_mysqldriver ${kexidb_mysqldriver_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kexidb_mysqldriver )

target_link_libraries(kexidb_mysqldriver  ${KDE4_KDECORE_LIBS} ${KDE4_KPARTS_LIBS} mysqlclient kexidb )

install_targets(${PLUGIN_INSTALL_DIR} kexidb_mysqldriver )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES  kexidb_mysqldriver.desktop )

kde4_footer()



#original Makefile.am contents follow:

#include $(top_srcdir)/kexi/Makefile.global
#
#kde_module_LTLIBRARIES = kexidb_mysqldriver.la
#
#INCLUDES = -I$(MYSQL_INC) -I$(srcdir)/../../.. \
#  -I$(srcdir)/../..  \
#  -I$(top_srcdir)/kexi $(all_includes)
#
#kexidb_mysqldriver_la_METASOURCES = AUTO
#
#kexidb_mysqldriver_la_SOURCES = \
#  mysqldriver.cpp \
#  mysqlconnection.cpp \
#  mysqlconnection_p.cpp \
#  mysqlcursor.cpp \
#  mysqlkeywords.cpp \
#  mysqlpreparedstatement.cpp
#
#kexidb_mysqldriver_la_LIBADD = $(LIB_KPARTS) \
#  $(LIB_QT) \
#  $(MYSQL_LIBS) \
#  -lmysqlclient \
#  ../../libkexidb.la
#
#kexidb_mysqldriver_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN) $(VER_INFO) -no-undefined
#
#
#kde_services_DATA = kexidb_mysqldriver.desktop
#
#
#KDE_CXXFLAGS += -DKEXIDB_MYSQL_DRIVER_EXPORT= -D__KEXIDB__= \
#	-include $(top_srcdir)/kexi/kexidb/global.h
#
