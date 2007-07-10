include_directories( ${POSTGRESQL_INCLUDE_DIR} ${PQXX_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/kexi ${KDE4_INCLUDES}  )

ADD_DEFINITIONS( ${KDE4_ENABLE_EXCEPTIONS} )

########### next target ###############

set(keximigrate_pqxx_PART_SRCS pqxxmigrate.cpp )

kde4_add_plugin(keximigrate_pqxx ${keximigrate_pqxx_PART_SRCS})



target_link_libraries(keximigrate_pqxx ${POSTGRESQL_LIBRARIES} ${PQXX_LIBRARIES} 
 ${KDE4_KDECORE_LIBS} ${KDE4_KPARTS_LIBS} keximigrate )

install(TARGETS keximigrate_pqxx  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  keximigrate_pqxx.desktop  DESTINATION ${SERVICES_INSTALL_DIR})





#original Makefile.am contents follow:

#include $(top_srcdir)/kexi/Makefile.global
#
#kde_module_LTLIBRARIES = keximigrate_pqxx.la
#
#INCLUDES = -I$(srcdir)/../../.. -I$(top_srcdir)/kexi $(all_includes) -I$(PG_INCDIR) -I$(PQXX_INCDIR)
#
#keximigrate_pqxx_la_METASOURCES = AUTO
#
#keximigrate_pqxx_la_SOURCES = pqxxmigrate.cpp
#
##TODO share -libs with pqxx kexidb drv!
##keximigrate_pqxx_la_LIBADD = $(LIB_KPARTS) $(LIB_QT) -lcom_err -lkrb5 -lssl -lcrypto -lcrypt -lpqxx ../libkeximigrate.la
#keximigrate_pqxx_la_LIBADD = $(LIB_KPARTS) $(LIB_QT) -lpqxx ../libkeximigrate.la
#
#keximigrate_pqxx_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN) -L$(PQXX_LIBDIR) -L$(PG_LIBDIR) $(VER_INFO) -no-undefined
#
#kde_services_DATA = keximigrate_pqxx.desktop
#
#noinst_HEADERS =  pqxxmigrate.h pg_type.h
#
