kde4_header()

include_directories( ${CMAKE_SOURCE_DIR}/kexi/core ${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/kexi/widget ${CMAKE_SOURCE_DIR}/kexi/migration ${CMAKE_SOURCE_DIR}/kexi/kexiDB ${KDE4_INCLUDES}  )


########### next target ###############

set(kexihandler_migration_PART_SRCS keximigrationpart.cpp )

kde4_automoc(${kexihandler_migration_PART_SRCS})

kde4_add_plugin(kexihandler_migration ${kexihandler_migration_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kexihandler_migration )

target_link_libraries(kexihandler_migration  ${KDE4_KDECORE_LIBS} kexicore keximigrate )

install_targets(${PLUGIN_INSTALL_DIR} kexihandler_migration )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR}/kexi FILES  keximigrationhandler.desktop )

kde4_footer()



#original Makefile.am contents follow:

#include $(top_srcdir)/kexi/Makefile.global
#
#kde_module_LTLIBRARIES = kexihandler_migration.la
#
#kexihandler_migration_la_SOURCES = keximigrationpart.cpp
#
#kexihandler_migration_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) $(VER_INFO) -module
#kexihandler_migration_la_LIBADD = ../../core/libkexicore.la \
#	../../migration/libkeximigrate.la
#
#INCLUDES= -I$(top_srcdir)/kexi/core -I$(top_srcdir)/kexi \
#		-I$(top_srcdir)/kexi/widget -I$(top_srcdir)/kexi/migration \
#		-I$(top_srcdir)/kexi/kexiDB $(all_includes)
#
#METASOURCES = AUTO
#
#servicesdir=$(kde_servicesdir)/kexi
#services_DATA=keximigrationhandler.desktop
#
#include ../Makefile.common
