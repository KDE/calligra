

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

