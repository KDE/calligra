

include_directories( ${MYSQL_INCLUDE_DIR} 
${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/kexi/kexidb ${KDE4_INCLUDES}  )

ADD_DEFINITIONS( -D__KEXIDB__= )

########### next target ###############

set(kexidb_mysqldriver_PART_SRCS 
   mysqldriver.cpp 
   mysqlconnection.cpp 
   mysqlconnection_p.cpp 
   mysqlcursor.cpp 
   mysqlkeywords.cpp 
   mysqlpreparedstatement.cpp )


kde4_add_plugin(kexidb_mysqldriver ${kexidb_mysqldriver_PART_SRCS})



target_link_libraries(kexidb_mysqldriver ${MYSQL_LIBRARIES} 
 ${KDE4_KDECORE_LIBS} ${CALLIGRADB_LIBS} kexidb)

install(TARGETS kexidb_mysqldriver  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kexidb_mysqldriver.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
