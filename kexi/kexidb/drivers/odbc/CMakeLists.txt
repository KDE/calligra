

include_directories( /usr/include/ ${CMAKE_SOURCE_DIR}/kexi ${KDE4_INCLUDES}  )

ADD_DEFINITIONS( -D__KEXIDB__= )

########### next target ###############

set(kexidb_odbcdriver_PART_SRCS kodbcdriver.cpp
				odbcdriver.cpp 
				odbcconnection_p.cpp 
				odbcconnection.cpp 
				odbccursor.cpp 
				odbcpreparedstatement.cpp
				odbcqueryunit.cpp
				odbcsqlqueryunit.cpp
				odbctypeinfoqueryunit.cpp
				odbctablesqueryunit.cpp
				odbcspecialcolumnsqueryunit.cpp
				)

kde4_add_plugin(kexidb_odbcdriver ${kexidb_odbcdriver_PART_SRCS})

target_link_libraries(kexidb_odbcdriver ${KDE4_KPARTS_LIBS} odbc kexidb )

install(TARGETS kexidb_odbcdriver  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kexidb_odbcdriver.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
