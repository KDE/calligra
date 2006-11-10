

include_directories( ${MYSQL_INCLUDE_DIR} 
${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/kexi/kexidb ${KDE4_INCLUDES}  )

ADD_DEFINITIONS( -D__KEXIDB__= -include ${CMAKE_SOURCE_DIR}/kexi/kexidb/global.h )

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



target_link_libraries(kexidb_mysqldriver ${MYSQL_LIBRARIES} 
 ${KDE4_KDECORE_LIBS} ${KDE4_KPARTS_LIBS} kexidb )

install(TARGETS kexidb_mysqldriver  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kexidb_mysqldriver.desktop  DESTINATION ${SERVICES_INSTALL_DIR})





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
