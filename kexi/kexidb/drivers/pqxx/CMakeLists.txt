

include_directories( ${POSTGRESQL_INCLUDE_DIR} ${PQXX_INCLUDE_DIR} 
${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/kexi/kexidb ${KDE4_INCLUDES}  )

ADD_DEFINITIONS( -D__KEXIDB__= ${KDE4_ENABLE_EXCEPTIONS} )

########### next target ###############

set(kexidb_pqxxsqldriver_PART_SRCS 
   pqxxdriver.cpp 
   pqxxcursor.cpp 
   pqxxconnection.cpp 
   pqxxkeywords.cpp 
   pqxxconnection_p.cpp 
   pqxxpreparedstatement.cpp )


kde4_add_plugin(kexidb_pqxxsqldriver ${kexidb_pqxxsqldriver_PART_SRCS})



target_link_libraries(kexidb_pqxxsqldriver ${POSTGRESQL_LIBRARIES} ${PQXX_LIBRARIES} 
 ${KDE4_KDECORE_LIBS} ${CALLIGRADB_LIBS} kexidb)

install(TARGETS kexidb_pqxxsqldriver  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kexidb_pqxxsqldriver.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
