

include_directories( ${POSTGRESQL_INCLUDE_DIR} ${PQXX_INCLUDE_DIR} 
${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/kexi/kexidb ${KDE4_INCLUDES}  )

ADD_DEFINITIONS( -D__KEXIDB__= ${KDE4_ENABLE_EXCEPTIONS} )

########### next target ###############

set(kexidb_pqxxsqldriver_PART_SRCS 
   pqxxdriver.cpp 
   pqxxcursor.cpp 
   pqxxconnection.cpp 
   pqxxkeywords.cpp 
   pqxxconnection_p.cpp 
   pqxxpreparedstatement.cpp )


kde4_add_plugin(kexidb_pqxxsqldriver ${kexidb_pqxxsqldriver_PART_SRCS})



target_link_libraries(kexidb_pqxxsqldriver ${POSTGRESQL_LIBRARIES} ${PQXX_LIBRARIES} 
 ${KDE4_KDECORE_LIBS} ${KDE4_KPARTS_LIBS} kexidb )

install(TARGETS kexidb_pqxxsqldriver  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kexidb_pqxxsqldriver.desktop  DESTINATION ${SERVICES_INSTALL_DIR})





#original Makefile.am contents follow:

#include $(top_srcdir)/kexi/Makefile.global
#
#kde_module_LTLIBRARIES = kexidb_pqxxsqldriver.la
#
#INCLUDES = -I$(srcdir)/../../.. $(all_includes) -I$(PG_INCDIR) -I$(PQXX_INCDIR)
#
#kexidb_pqxxsqldriver_la_METASOURCES = AUTO
#
#kexidb_pqxxsqldriver_la_SOURCES = pqxxdriver.cpp pqxxcursor.cpp pqxxconnection.cpp \
#	pqxxkeywords.cpp pqxxconnection_p.cpp pqxxpreparedstatement.cpp
#
#kexidb_pqxxsqldriver_la_LIBADD = $(LIB_KPARTS) $(LIB_QT) -lpqxx ../../libkexidb.la
#
#kexidb_pqxxsqldriver_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN) \
#	-L$(PQXX_LIBDIR) -L$(PG_LIBDIR) $(VER_INFO) -no-undefined
#
#kde_services_DATA = kexidb_pqxxsqldriver.desktop
#
#noinst_HEADERS =  pqxxconnection.h pqxxconnection_p.h
#
#KDE_CXXFLAGS += -DKEXIDB_PGSQL_DRIVER_EXPORT= -D__KEXIDB__= \
#	-include $(top_srcdir)/kexi/kexidb/global.h
