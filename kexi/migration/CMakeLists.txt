include_directories( ${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/kexi/core
  ${CMAKE_SOURCE_DIR}/kexi/widget ${CMAKE_BINARY_DIR}/kexi/widget
  
)

add_definitions(-DKDE_DEFAULT_DEBUG_AREA=44000)

add_subdirectory(txt)

if (MYSQL_FOUND)
  add_subdirectory( mysql )
endif ()

if (POSTGRESQL_FOUND AND PQXX_FOUND AND PQXX_VERSION VERSION_GREATER PQXX_MIN_VERSION AND PQXX_VERSION VERSION_LESS PQXX_MAX_VERSION)
  add_subdirectory( pqxx )
endif ()

if(FREETDS_FOUND)
  add_subdirectory(sybase)
endif()

if(XBASE_FOUND)
  add_subdirectory(xbase)
endif()

macro_optional_find_package(GLIB2)

if (GLIB2_FOUND)
    add_subdirectory( mdb )
endif()

add_subdirectory(tests)

########### next target ###############

set(keximigrate_LIB_SRCS AlterSchemaTableModel.cpp 
   keximigrate.cpp 
   keximigratedata.cpp 
   migratemanager.cpp 
   importwizard.cpp
   importtablewizard.cpp
   importoptionsdlg.cpp
   AlterSchemaWidget.cpp)

add_library(keximigrate SHARED ${keximigrate_LIB_SRCS})

target_link_libraries(
    keximigrate
    kexicore
    kexiextendedwidgets
    kexiutils

    KDb

    Qt5::Gui
    )

set_target_properties(keximigrate PROPERTIES VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION} )
install(TARGETS keximigrate  ${INSTALL_TARGETS_DEFAULT_ARGS})

########### install files ###############

if(FALSE) # TODO: install when we move to independent place
    install( FILES  keximigrate.h keximigratedata.h migratemanager.h  DESTINATION ${INCLUDE_INSTALL_DIR}/kexidb COMPONENT Devel)
endif()

install( FILES  keximigration_driver.desktop  DESTINATION ${SERVICETYPES_INSTALL_DIR})


