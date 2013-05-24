

include_directories( ${FreeTDS_INCLUDE_DIR} 
${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/kexi/kexidb ${KDE4_INCLUDES}  )

ADD_DEFINITIONS( -D__KEXIDB__= )

########### next target ###############

set(kexidb_sybasedriver_PART_SRCS 
   sybasedriver.cpp 
   sybaseconnection.cpp 
   sybaseconnection_p.cpp 
   sybasecursor.cpp 
   sybasekeywords.cpp 
   sybasepreparedstatement.cpp )


kde4_add_plugin(kexidb_sybasedriver ${kexidb_sybasedriver_PART_SRCS})

target_link_libraries(kexidb_sybasedriver ${FreeTDS_LIBRARIES}
 ${KDE4_KDECORE_LIBS} ${CALLIGRADB_LIBS} kexidb)

install(TARGETS kexidb_sybasedriver  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kexidb_sybasedriver.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
