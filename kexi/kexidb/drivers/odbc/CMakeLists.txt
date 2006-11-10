

include_directories( ${CMAKE_SOURCE_DIR}/kexi ${KDE4_INCLUDES}  )


########### next target ###############

set(kexidb_odbcdriver_PART_SRCS odbcdriver.cpp odbcconnection.cpp )

kde4_automoc(${kexidb_odbcdriver_PART_SRCS})

kde4_add_plugin(kexidb_odbcdriver ${kexidb_odbcdriver_PART_SRCS})



target_link_libraries(kexidb_odbcdriver  ${KDE4_KDECORE_LIBS} ${KDE4_KPARTS_LIBS} odbc kexidb )

install(TARGETS kexidb_odbcdriver  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kexidb_odbcdriver.desktop  DESTINATION ${SERVICES_INSTALL_DIR})





#original Makefile.am contents follow:

#include $(top_srcdir)/kexi/Makefile.global
#
#kde_module_LTLIBRARIES = kexidb_odbcdriver.la
#
#INCLUDES = -I$(srcdir)/../..  -I$(top_srcdir)/kexi $(all_includes)
#
#kexidb_odbcdriver_la_METASOURCES = AUTO
#
#kexidb_odbcdriver_la_SOURCES = odbcdriver.cpp odbcconnection.cpp 
#
#kexidb_odbcdriver_la_LIBADD = $(LIB_KPARTS) $(LIB_QT) -lodbc $(top_builddir)/kexi/kexidb/libkexidb.la
#
#kexidb_odbcdriver_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN) $(VER_INFO) -no-undefined
#
#
#kde_services_DATA = kexidb_odbcdriver.desktop
#
#
#KDE_CXXFLAGS += -DKEXIDB_ODBC_DRIVER_EXPORT= -D__KEXIDB__= \
#	-include $(top_srcdir)/kexi/kexidb/global.h
#
