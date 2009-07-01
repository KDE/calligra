

add_definitions(${WV2_INCLUDE_DIR} -DQT3_SUPPORT -DQT3_SUPPORT_WARNINGS)



########### next target ###############

set(mswordodf_import_PART_SRCS
   mswordodfimport.cpp
   document.cpp
   conversion.cpp
   texthandler.cpp
   paragraph.cpp
   tablehandler.cpp
   graphicshandler.cpp )


kde4_add_plugin(mswordodf_import WITH_PREFIX ${mswordodf_import_PART_SRCS})



target_link_libraries(mswordodf_import ${QT_QTGUI_LIBRARY} koodf kostore ${QT_QT3SUPPORT_LIBRARY} ${KDE4_KDECORE_LIBS} ${WV2_LIBRARIES} komain)

install(TARGETS mswordodf_import  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kword_msword-odf_import.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

