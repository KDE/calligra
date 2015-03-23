include_directories( ${CMAKE_SOURCE_DIR}/kexi   )

set(krossmodulekexidb_PART_SRCS 
   kexidbmodule.cpp
   kexidbfield.cpp 
   kexidbfieldlist.cpp 
   kexidbschema.cpp 
   kexidbparser.cpp 
   kexidbcursor.cpp 
   kexidbconnectiondata.cpp 
   kexidbconnection.cpp 
   kexidbdriver.cpp )

add_library(krossmodulekexidb MODULE ${krossmodulekexidb_PART_SRCS})

target_link_libraries(krossmodulekexidb Predicate)
install(TARGETS krossmodulekexidb DESTINATION ${PLUGIN_INSTALL_DIR})
