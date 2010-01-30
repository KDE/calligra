project(kexi)

SET(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true)

IF(EXISTS ${CMAKE_SOURCE_DIR}/kexi/CMakeSettings.txt)
  INCLUDE(${CMAKE_SOURCE_DIR}/kexi/CMakeSettings.txt)
ENDIF(EXISTS ${CMAKE_SOURCE_DIR}/kexi/CMakeSettings.txt)

ADD_DEFINITIONS(
  -DKEXI_NO_CURSOR_PROPERTY 
  -DKEXI_NO_CTXT_HELP
  -DKEXI_NO_SUBFORM
  -DKEXI_DB_COMBOBOX_WIDGET
  -DDB_TEMPLATES
  -DKEXI_NO_UNFINISHED
  -DKEXI_SCRIPTS_SUPPORT
)

#no default: add_definitions(-DKDE_DEFAULT_DEBUG_AREA=44010)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config-kexi.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-kexi.h )

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${KOMAIN_INCLUDES}
  ${KDE4_INCLUDES}
  ${QT_INCLUDES}
  ${CMAKE_SOURCE_DIR}/libs
)

add_definitions(-DQT3_SUPPORT -DQT3_SUPPORT_WARNINGS)

SET(KOPROPERTY_TARGETNAME koproperty)                           # TODO remove when we move to independent place
SET(KOPROPERTY_LIBS ${KOPROPERTY_TARGETNAME})                   # TODO remove when we move to independent place
SET(KOPROPERTY_LIB_VERSION ${GENERIC_KOFFICE_LIB_VERSION})      # TODO remove when we move to independent place
SET(KOPROPERTY_LIB_SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION})  # TODO remove when we move to independent place
SET(KOPROPERTY_USE_KOLIBS TRUE)                                 # TODO remove when we move to independent place
add_subdirectory( koproperty )

add_subdirectory( kexiutils )
add_subdirectory( kexidb )
add_subdirectory( core )
add_subdirectory( widget )
add_subdirectory( formeditor )

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
    macro_optional_add_subdirectory(webforms)
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

add_subdirectory( data )
add_subdirectory( pics )

add_subdirectory( main )

add_subdirectory( migration )
add_subdirectory( plugins )
add_subdirectory( tests )

add_subdirectory(shapes)

########### next target ###############

set(kexi_SRCS main.cpp )
kde4_add_app_icon(kexi_SRCS "${KDE4_ICON_DIR}/oxygen/*/apps/kexi.png")
kde4_add_executable(kexi ${kexi_SRCS})
target_link_libraries(kexi 
  ${KDE4_KFILE_LIBS} ${QT_QT3SUPPORT_LIBRARY}
  kexidb
  keximain
)
install(TARGETS kexi ${INSTALL_TARGETS_DEFAULT_ARGS})

########### install files ###############

install(FILES kexi.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})

#kde4_create_doxygen_docs( EXCLUDE 3rdparty kexidb/parser/sqlparser.h kexidb/drivers/odbc  )
