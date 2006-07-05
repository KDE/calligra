

add_definitions(${WV2_INCLUDE_DIR})


########### next target ###############

set(mswordimport_PART_SRCS 
   mswordimport.cpp 
   document.cpp 
   conversion.cpp 
   texthandler.cpp 
   tablehandler.cpp 
   graphicshandler.cpp )

kde4_automoc(${mswordimport_PART_SRCS})

kde4_add_plugin(mswordimport WITH_PREFIX ${mswordimport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} mswordimport )

target_link_libraries(mswordimport  ${KDE4_KDECORE_LIBS} ${WV2_LIBRARIES} kofficecore)

install(TARGETS mswordimport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kword_msword_import.desktop  DESTINATION ${SERVICES_INSTALL_DIR})





#original Makefile.am contents follow:

######## General stuff
#
#INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) $(LIBWV2_CFLAGS) $(all_includes)
#
######## Files
#
#kde_module_LTLIBRARIES = libmswordimport.la
#
#libmswordimport_la_SOURCES = mswordimport.cpp document.cpp conversion.cpp \
#	texthandler.cpp tablehandler.cpp graphicshandler.cpp
#libmswordimport_la_LDFLAGS = -module $(KDE_PLUGIN)
#libmswordimport_la_LIBADD = $(KOFFICE_LIBS) $(LIBWV2_LIBS)
#
#METASOURCES = AUTO
#
#service_DATA = kword_msword_import.desktop
#servicedir = $(kde_servicesdir)
