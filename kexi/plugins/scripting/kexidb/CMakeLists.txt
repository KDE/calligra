include_directories( ${CMAKE_SOURCE_DIR}/kexi ${KDE4_INCLUDES}  )

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

kde4_add_plugin(krossmodulekexidb ${krossmodulekexidb_PART_SRCS})

##target_link_libraries(krossmodulekexidb  ${KDE4_KDECORE_LIBS} kexidb kexidbparser )
target_link_libraries(krossmodulekexidb ${KDE4_KDECORE_LIBS} kexidb)
install(TARGETS krossmodulekexidb DESTINATION ${PLUGIN_INSTALL_DIR})
