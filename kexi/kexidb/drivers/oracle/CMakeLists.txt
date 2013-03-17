

include_directories( ${ORACLE_INCLUDE_DIR} 
${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/kexi/kexidb ${KDE4_INCLUDES}  )

#ADD_DEFINITIONS( -D__KEXIDB__= )
ADD_DEFINITIONS( -D__KEXIDB__= ${KDE4_ENABLE_EXCEPTIONS} )

########### next target ###############

set(kexidb_oracledriver_PART_SRCS 
   oracledriver.cpp 
   oracleconnection.cpp 
   oracleconnection_p.cpp 
   oraclecursor.cpp 
   oraclekeywords.cpp 
   oraclepreparedstatement.cpp )

INCLUDE(/usr/include/oracle/11.1.0.1/client )
kde4_add_plugin(kexidb_oracledriver ${kexidb_oracledriver_PART_SRCS})



target_link_libraries(kexidb_oracledriver ${ORACLE_LIBRARIES} 
 ${KDE4_KDECORE_LIBS} kexidb occi clntsh)

install(TARGETS kexidb_oracledriver  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kexidb_oracledriver.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
