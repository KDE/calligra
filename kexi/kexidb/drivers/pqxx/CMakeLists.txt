

include_directories( ${PostgreSQL_INCLUDE_DIRS} ${PQXX_INCLUDE_DIR}
${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/kexi/kexidb ${KDE4_INCLUDES}  )

add_definitions( -D__KEXIDB__= ${KDE4_ENABLE_EXCEPTIONS} )

########### next target ###############

set(kexidb_pqxxsqldriver_PART_SRCS 
   pqxxdriver.cpp 
   pqxxcursor.cpp 
   pqxxconnection.cpp 
   pqxxkeywords.cpp 
   pqxxconnection_p.cpp 
   pqxxpreparedstatement.cpp )


add_library(kexidb_pqxxsqldriver MODULE ${kexidb_pqxxsqldriver_PART_SRCS})



target_link_libraries(kexidb_pqxxsqldriver ${POSTGRESQL_LIBRARIES} ${PQXX_LIBRARIES} 
  ${CALLIGRADB_LIBS} kexidb)

install(TARGETS kexidb_pqxxsqldriver  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kexidb_pqxxsqldriver.desktop  DESTINATION ${SERVICES_INSTALL_DIR}/calligra)
