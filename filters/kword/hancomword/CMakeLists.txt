


add_definitions(-DQT3_SUPPORT -DQT3_SUPPORT_WARNINGS)


########### next target ###############

set(hancomwordimport_PART_SRCS pole.cpp hancomwordimport.cpp )


kde4_add_plugin(hancomwordimport WITH_PREFIX ${hancomwordimport_PART_SRCS})



target_link_libraries(hancomwordimport komain)

install(TARGETS hancomwordimport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kword_hancomword_import.desktop  DESTINATION ${SERVICES_INSTALL_DIR})





#original Makefile.am contents follow:

######## General stuff
#
#INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) $(all_includes)
#
######## Files
#
#kde_module_LTLIBRARIES = libhancomwordimport.la
#
#libhancomwordimport_la_SOURCES = pole.cpp hancomwordimport.cpp
#libhancomwordimport_la_LIBADD = $(KOFFICE_LIBS) 
#libhancomwordimport_la_LDFLAGS = -module $(KDE_PLUGIN)
#noinst_HEADERS = pole.h hancomwordimport.h
#
#METASOURCES = AUTO
#
#service_DATA = kword_hancomword_import.desktop
#servicedir = $(kde_servicesdir)
