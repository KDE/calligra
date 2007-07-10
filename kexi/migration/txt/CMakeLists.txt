

include_directories( ${CMAKE_SOURCE_DIR}/kexi ${KDE4_INCLUDES}  )


########### next target ###############

set(keximigrate_txtmigrate_PART_SRCS txtmigrate.cpp )

kde4_automoc(${keximigrate_txtmigrate_PART_SRCS})

kde4_add_plugin(keximigrate_txtmigrate ${keximigrate_txtmigrate_PART_SRCS})



target_link_libraries(keximigrate_txtmigrate  ${KDE4_KPARTS_LIBS} keximigrate )

install(TARGETS keximigrate_txtmigrate  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  keximigrate_txtmigrate.desktop  DESTINATION ${SERVICES_INSTALL_DIR})





#original Makefile.am contents follow:

#include $(top_srcdir)/kexi/Makefile.global
#
#kde_module_LTLIBRARIES = keximigrate_txtmigrate.la
#
#INCLUDES = -I$(srcdir)/../../.. -I$(top_srcdir)/kexi $(all_includes)
#
#keximigrate_txtmigrate_la_METASOURCES = AUTO
#
#keximigrate_txtmigrate_la_SOURCES = txtmigrate.cpp
#
#keximigrate_txtmigrate_la_LIBADD = $(LIB_KPARTS) $(LIB_QT) ../libkeximigrate.la
#
#keximigrate_txtmigrate_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN) $(VER_INFO) -no-undefined
#
#kde_services_DATA = keximigrate_txtmigrate.desktop
#
#noinst_HEADERS =  txtmigrate.h
#
