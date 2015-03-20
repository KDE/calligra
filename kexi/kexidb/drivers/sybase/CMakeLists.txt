

include_directories( ${FreeTDS_INCLUDE_DIR} 
${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/kexi/kexidb   )

add_definitions( -D__KEXIDB__= )

########### next target ###############

set(kexidb_sybasedriver_PART_SRCS 
   sybasedriver.cpp 
   sybaseconnection.cpp 
   sybaseconnection_p.cpp 
   sybasecursor.cpp 
   sybasekeywords.cpp 
   sybasepreparedstatement.cpp )


add_library(kexidb_sybasedriver MODULE ${kexidb_sybasedriver_PART_SRCS})

target_link_libraries(kexidb_sybasedriver ${FreeTDS_LIBRARIES}
  ${CALLIGRADB_LIBS} kexidb)

install(TARGETS kexidb_sybasedriver  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kexidb_sybasedriver.desktop  DESTINATION ${SERVICES_INSTALL_DIR}/calligra)
