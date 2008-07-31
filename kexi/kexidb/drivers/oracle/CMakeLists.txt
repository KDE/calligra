

include_directories( ${ORACLE_INCLUDE_DIR} 
${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/kexi/kexidb ${KDE4_INCLUDES}  )

ADD_DEFINITIONS( -D__KEXIDB__= )

########### next target ###############

set(kexidb_oracledriver_PART_SRCS 
   oracledriver.cpp 
   oracleconnection.cpp 
   oracleconnection_p.cpp 
   oraclecursor.cpp 
   oraclekeywords.cpp 
   oraclepreparedstatement.cpp )

INCLUDE(/usr/include/oracle/11.1.0.1/client )
kde4_add_plugin(kexidb_oracledriver ${kexidb_oracledriver_PART_SRCS})



target_link_libraries(kexidb_oracledriver ${ORACLE_LIBRARIES} 
 ${KDE4_KDECORE_LIBS} ${KDE4_KPARTS_LIBS} kexidb occi clntsh)

install(TARGETS kexidb_oracledriver  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kexidb_oracledriver.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

#original Makefile.am contents follow:

#include $(top_srcdir)/kexi/Makefile.global
#
#kde_module_LTLIBRARIES = kexidb_oracledriver.la
#
#INCLUDES = -I$(ORACLE_INC) -I$(srcdir)/../../.. \
#  -I$(srcdir)/../..  \
#  -I$(top_srcdir)/kexi $(all_includes) \
#  -I/usr/include/oracle/11.1.0.1/client 
#
#kexidb_oracledriver_la_METASOURCES = AUTO
#
#kexidb_oracledriver_la_SOURCES = \
#  oracledriver.cpp \
#  oracleconnection.cpp \
#  oracleconnection_p.cpp \
#  oraclecursor.cpp \
#  oraclekeywords.cpp \
#  oraclepreparedstatement.cpp
#
#kexidb_oracledriver_la_LIBADD = $(LIB_KPARTS) \
#  $(LIB_QT) \
#  $(ORACLE_LIBS) \
#  -locci \
#  -lclntsh \
#  -L/usr/lib/oracle/11.1.0.1/client/lib \
#  ../../libkexidb.la
#
#kexidb_oracledriver_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN) $(VER_INFO) -no-undefined
#
#
#kde_services_DATA = kexidb_oracledriver.desktop
#
#
#KDE_CXXFLAGS += -DKEXIDB_ORACLE_DRIVER_EXPORT= -D__KEXIDB__= \
#	-include $(top_srcdir)/kexi/kexidb/global.h
#
