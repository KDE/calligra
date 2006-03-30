kde4_header()




########### next target ###############

set(hancomwordimport_PART_SRCS pole.cpp hancomwordimport.cpp )

kde4_automoc(${hancomwordimport_PART_SRCS})

kde4_add_plugin(hancomwordimport WITH_PREFIX ${hancomwordimport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} hancomwordimport )

target_link_libraries(hancomwordimport  ${KDE4_KDECORE_LIBS} kstore kofficecore)

install_targets(${LIB_INSTALL_DIR}/kde4 hancomwordimport )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES  kword_hancomword_import.desktop )

kde4_footer()



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
