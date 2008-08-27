

add_definitions(${WV2_INCLUDE_DIR})


########### next target ###############

set(mswordimport_PART_SRCS 
   mswordimport.cpp 
   document.cpp 
   conversion.cpp 
   texthandler.cpp 
   tablehandler.cpp 
   graphicshandler.cpp )


kde4_add_plugin(mswordimport WITH_PREFIX ${mswordimport_PART_SRCS})



target_link_libraries(mswordimport ${QT_QT3SUPPORT_LIBRARY} ${QT_QTGUI_LIBRARY} ${KDE4_KDECORE_LIBS} ${WV2_LIBRARIES} komain ${QT_QTXML_LIBRARY} )

install(TARGETS mswordimport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kword_msword_import.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

