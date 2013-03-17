project(kexi)

INCLUDE(CheckFunctionExists)

SET(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true)

option(KEXI_MOBILE "compile a mobile Kexi" OFF)

check_function_exists("uname" HAVE_UNAME)

IF(EXISTS ${CMAKE_SOURCE_DIR}/kexi/CMakeSettings.txt)
  INCLUDE(${CMAKE_SOURCE_DIR}/kexi/CMakeSettings.txt)
ENDIF(EXISTS ${CMAKE_SOURCE_DIR}/kexi/CMakeSettings.txt)

option(KEXI_DEBUG_GUI "Debugging GUI for Kexi (requires CALLIGRADB_DEBUG_GUI to be set too)" OFF)

if(KEXI_AUTORISE_TABBED_TOOLBAR)
    ADD_DEFINITIONS(-DKEXI_AUTORISE_TABBED_TOOLBAR)
endif(KEXI_AUTORISE_TABBED_TOOLBAR)

if(NOT KEXI_UNFINISHED)
  ADD_DEFINITIONS(-DKEXI_NO_UNFINISHED)
endif(NOT KEXI_UNFINISHED)

if(KEXI_SHOW_UNIMPLEMENTED)
  ADD_DEFINITIONS(-DKEXI_SHOW_UNIMPLEMENTED)
endif(KEXI_SHOW_UNIMPLEMENTED)

ADD_DEFINITIONS(
  -DKEXI_NO_CURSOR_PROPERTY 
  -DKEXI_NO_CTXT_HELP
  -DKEXI_NO_SUBFORM
  -DKEXI_DB_COMBOBOX_WIDGET
  -DDB_TEMPLATES
  -DKEXI_SCRIPTS_SUPPORT
  -DKEXI_NO_QUICK_PRINTING # print/print preview for tables/queries in the navigator
# temporary for 2.2:
  -DKEXI_NO_AUTOFIELD_WIDGET
  -DKEXI_NO_FORM_LAYOUTS
  -DKEXI_NO_FORM_SPRING_ELEMENT
)

#no default: add_definitions(-DKDE_DEFAULT_DEBUG_AREA=44010)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config-kexi.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-kexi.h )

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${KOMAIN_INCLUDES}
  ${KOPROPERTY_INCLUDES}
  ${CALLIGRADB_INCLUDES}
  ${CMAKE_SOURCE_DIR}/libs
)

IF(KEXI_MOBILE)
  add_definitions(-DKEXI_MOBILE)
ELSE(KEXI_MOBILE)
  add_definitions(-DQT3_SUPPORT -DQT3_SUPPORT_WARNINGS)
ENDIF(KEXI_MOBILE)

if(FALSE) #TODO ENABLE WEBFORMS
if(WITH_WEBFORMS)
  macro_optional_find_package(PionNet)
  macro_log_feature(PionNet_FOUND "Pion Network Library" "C++ development library for implementing lightweight HTTP interfaces" "http://www.pion.org/projects/pion-network-library" FALSE "" "Required by Kexi Web Forms")

  macro_optional_find_package(GoogleCtemplate)
  macro_log_feature(GOOGLE_CTEMPLATE_FOUND "Google Ctemplate" "Simple but powerful template language for C++" "http://code.google.com/p/google-ctemplate/" FALSE "" "Required by Kexi Web Forms")
  
  macro_log_feature(Boost_FOUND "Boost" "Boost C++ Libraries" "http://www.boost.org" FALSE "" "Required by Kexi Web Forms")

  if(GOOGLE_CTEMPLATE_FOUND AND Boost_FOUND AND PionNet_FOUND)
    set(WITH_WEBFORMS TRUE CACHE STRING "Build with Kexi Web Forms support")
    add_definitions(-DKEXIWEBFORMS_SUPPORTED)
    message(STATUS "Kexi Web Forms support enabled.")
    add_subdirectory(webforms)
  else(GOOGLE_CTEMPLATE_FOUND AND Boost_FOUND AND PionNet_FOUND)
    set(WITH_WEBFORMS FALSE CACHE STRING "Build with Kexi Web Forms support")
    message(STATUS "Kexi Web Forms support DISABLED, one of the following libraries is missing:")
    message(STATUS "- Google CTemplate")
    message(STATUS "- Boost")
    message(STATUS "- Pion Network Library")
  endif(GOOGLE_CTEMPLATE_FOUND AND Boost_FOUND AND PionNet_FOUND)
else(WITH_WEBFORMS)
  set(WITH_WEBFORMS FALSE CACHE STRING "Build with Kexi Web Forms support")
endif(WITH_WEBFORMS)
endif(FALSE)

add_subdirectory( kexiutils )

macro_optional_find_package(MySQL)
macro_log_feature(MYSQL_FOUND "libmysqlclient" "MySQL Client Library" "http://www.mysql.com" FALSE "" "Required by Kexi MySQL driver")

INCLUDE (FindPostgreSQL)

set (PQXX_MIN_VERSION "3.0.0")
set (PQXX_MAX_VERSION "4.0.0")

macro_optional_find_package(PostgreSQL)
macro_log_feature(POSTGRESQL_FOUND "libpq" "C application programmer's interface to PostgreSQL" "http://www.postgresql.org" FALSE "" "Required by Kexi PostgreSQL driver")

pkg_check_modules (PQXX libpqxx)

IF (POSTGRESQL_FOUND AND PQXX_FOUND)
   IF (PQXX_VERSION VERSION_GREATER PQXX_MIN_VERSION AND PQXX_VERSION VERSION_LESS PQXX_MAX_VERSION)
      macro_log_feature(PQXX_FOUND "libpqxx" "Official C++ client API for PostgreSQL" "http://pqxx.org/development/libpqxx/" FALSE "" "Required by Kexi PostgreSQL driver")
   ELSE (PQXX_VERSION VERSION_GREATER PQXX_MIN_VERSION AND PQXX_VERSION VERSION_LESS PQXX_MAX_VERSION)
      macro_log_feature(FALSE "libpqxx" "Official C++ client API for PostgreSQL, version >= ${PQXX_MIN_VERSION} and older than ${PQXX_MAX_VERSION}" "http://pqxx.org/development/libpqxx/" FALSE "" "Required by Kexi PostgreSQL driver")
   ENDIF (PQXX_VERSION VERSION_GREATER PQXX_MIN_VERSION AND PQXX_VERSION VERSION_LESS PQXX_MAX_VERSION)
ENDIF (POSTGRESQL_FOUND AND PQXX_FOUND)

macro_optional_find_package(FreeTDS)
macro_log_feature(FREETDS_FOUND "FreeTDS" "Open source implementation of the TDS (Tabular Data Stream) protocol" "http://www.freetds.org" FALSE "" "Required by Kexi Sybase driver")

macro_optional_find_package(XBase)
macro_log_feature(XBASE_FOUND "XBase" "XBase compatible C++ class library" "http://linux.techass.com/projects/xdb" FALSE "" "Required by Kexi XBase driver")

add_subdirectory( kexidb )
add_subdirectory( core )
add_subdirectory( widget )
add_subdirectory( data )
add_subdirectory( pics )
add_subdirectory( plugins )
add_subdirectory( tests )

IF(KEXI_MOBILE)

ELSE(KEXI_MOBILE)
  add_subdirectory( main )
  add_subdirectory( formeditor )
  add_subdirectory( migration )
ENDIF(KEXI_MOBILE)

########### next target ###############

IF(KEXI_MOBILE)
  add_subdirectory( mobile )
ELSE(KEXI_MOBILE)
  set(kexi_SRCS main.cpp )
  kde4_add_app_icon(kexi_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/pics/app/hi*-app-calligrakexi.png")
  kde4_add_executable(kexi ${kexi_SRCS})
  target_link_libraries(kexi 
    ${KDE4_KFILE_LIBS} ${QT_QT3SUPPORT_LIBRARY}
    ${CALLIGRADB_LIBS}
    keximain
    kexicore
  )
  install(TARGETS kexi ${INSTALL_TARGETS_DEFAULT_ARGS})
ENDIF(KEXI_MOBILE)

########### install files ###############

install(PROGRAMS kexi.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})

#kde4_create_doxygen_docs( EXCLUDE 3rdparty kexidb/parser/sqlparser.h kexidb/drivers/odbc  )
