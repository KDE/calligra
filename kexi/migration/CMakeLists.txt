include_directories( ${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/kexi/core
  ${CMAKE_SOURCE_DIR}/kexi/widget ${CMAKE_BINARY_DIR}/kexi/widget
  ${CMAKE_SOURCE_DIR}/kexi/main/startup ${CMAKE_BINARY_DIR}/kexi/main
  ${KDE4_INCLUDES}
)

add_definitions(-DKDE_DEFAULT_DEBUG_AREA=44000)

add_subdirectory(txt)

IF (MYSQL_FOUND)
  add_subdirectory( mysql )
ENDIF (MYSQL_FOUND)

IF (POSTGRESQL_FOUND AND PQXX_FOUND AND PQXX_VERSION VERSION_GREATER PQXX_MIN_VERSION AND PQXX_VERSION VERSION_LESS PQXX_MAX_VERSION)
  add_subdirectory( pqxx )
ENDIF (POSTGRESQL_FOUND AND PQXX_FOUND AND PQXX_VERSION VERSION_GREATER PQXX_MIN_VERSION AND PQXX_VERSION VERSION_LESS PQXX_MAX_VERSION)

IF(FREETDS_FOUND)
  add_subdirectory(sybase)
ENDIF(FREETDS_FOUND)

IF(XBASE_FOUND)
  add_subdirectory(xbase)
ENDIF(XBASE_FOUND)

macro_optional_find_package(GLIB2)

if (GLIB2_FOUND)
    add_subdirectory( mdb )
ENDIF(GLIB2_FOUND)

########### next target ###############

set(keximigrate_LIB_SRCS AlterSchemaTableModel.cpp 
   keximigrate.cpp 
   keximigratedata.cpp 
   migratemanager.cpp 
   importwizard.cpp
   importtablewizard.cpp
   importoptionsdlg.cpp
   AlterSchemaWidget.cpp)

kde4_add_library(keximigrate SHARED ${keximigrate_LIB_SRCS})

target_link_libraries(
    keximigrate
    kexicore
    kexidb
    kexiextendedwidgets
    keximain
    kexiutils
    ${CALLIGRADB_LIBS}
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KDEUI_LIBS}
    ${KDE4_KDE3SUPPORT_LIBS}
    ${QT_QTGUI_LIBRARY}
    )

set_target_properties(keximigrate PROPERTIES VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION} )
install(TARGETS keximigrate  ${INSTALL_TARGETS_DEFAULT_ARGS})


########### next target ###############

set(keximigratetest_SRCS keximigratetest.cpp )


kde4_add_executable(keximigratetest ${keximigratetest_SRCS})

target_link_libraries(keximigratetest  ${KDE4_KDECORE_LIBS} keximigrate kexicore kexidb kexiextendedwidgets keximain )

########### install files ###############

if(FALSE) # TODO: install when we move to independent place
    install( FILES  keximigrate.h keximigratedata.h migratemanager.h  DESTINATION ${INCLUDE_INSTALL_DIR}/kexidb COMPONENT Devel)
endif(FALSE)

install( FILES  keximigration_driver.desktop  DESTINATION ${SERVICETYPES_INSTALL_DIR})


