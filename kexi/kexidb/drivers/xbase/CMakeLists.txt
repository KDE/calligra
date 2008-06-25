

include_directories( ${XBASE_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/kexi/kexidb ${KDE4_INCLUDES}  )

ADD_DEFINITIONS( -D__KEXIDB__= )

########### next target ###############

set(kexidb_xbasedriver_PART_SRCS 
   xbasedriver.cpp 
   xbaseconnection.cpp 
   xbaseconnection_p.cpp 
   xbasecursor.cpp 
   xbasekeywords.cpp 
   )


kde4_add_plugin(kexidb_xbasedriver ${kexidb_xbasedriver_PART_SRCS})

target_link_libraries(kexidb_xbasedriver ${XBASE_LIBRARIES} 
 ${KDE4_KDECORE_LIBS} ${KDE4_KPARTS_LIBS} kexidb keximigrate)

install(TARGETS kexidb_xbasedriver  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kexidb_xbasedriver.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
