

include_directories( ${FreeTDS_INCLUDE_DIR} 
${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/kexi/kexidb ${KDE4_INCLUDES}  )

ADD_DEFINITIONS( -D__KEXIDB__= )

########### next target ###############

set(kexidb_sybasedriver_PART_SRCS 
   sybasedriver.cpp 
   sybaseconnection.cpp 
   sybaseconnection_p.cpp 
   sybasecursor.cpp 
   sybasekeywords.cpp 
   sybasepreparedstatement.cpp )


kde4_add_plugin(kexidb_sybasedriver ${kexidb_sybasedriver_PART_SRCS})

target_link_libraries(kexidb_sybasedriver ${FreeTDS_LIBRARIES}
 ${KDE4_KDECORE_LIBS} ${KDE4_KPARTS_LIBS} kexidb )

install(TARGETS kexidb_sybasedriver  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kexidb_sybasedriver.desktop  DESTINATION ${SERVICES_INSTALL_DIR})





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
