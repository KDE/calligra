project(kexi)

set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true)

option(KEXI_MOBILE "Compile a mobile version of Kexi" OFF)

check_function_exists("uname" HAVE_UNAME)

option(KEXI_DEBUG_GUI "Debugging GUI for Kexi (requires KDB_DEBUG_GUI to be set too)" OFF)
option(KEXI_SHOW_UNFINISHED, "Show unfinished features in Kexi. Thus is useful for testing but may confuse end-user." OFF)
option(KEXI_SHOW_UNIMPLEMENTED "Forces to show menu entries and dialogs just to give impression about development plans for Kexi. Only recommended for test/development versions." OFF)

# Extra GUI features
option(KEXI_AUTORISE_TABBED_TOOLBAR "Experimental: Autorise the main tabbed toolbar in Kexi" OFF)

# Experimental:
option(KEXI_SCRIPTS_SUPPORT "Experimental: Enable scripting in Kexi" ON)

# Broken:
option(KEXI_FORM_CURSOR_PROPERTY_SUPPORT "Broken: Enable \"cursor\" property in the form designer" OFF)
option(KEXI_SHOW_CONTEXT_HELP "Broken: Enable context help in Kexi main window" OFF)
option(KEXI_QUICK_PRINTING_SUPPORT "Broken: Enable print/print preview/print setup for tables/queries in the project navigator" OFF)
option(KEXI_AUTOFIELD_FORM_WIDGET_SUPPORT "Broken: Enable \"auto field\" form widget in the form designer" OFF)
# OFF because we need to replace it with QTreeWidget which uses very different API compared to Q3ListView. Re-add QTreeWidget?
option(KEXI_LIST_FORM_WIDGET_SUPPORT "Broken: Enable \"list\" form widget in the form designer" OFF)
option(KEXI_PIXMAP_COLLECTIONS_SUPPORT "Broken: Enable support for pixmap collections" OFF)

# Not available:
option(KEXI_MACROS_SUPPORT "Experimental: Enable macros in Kexi" OFF)
if(KEXI_MACROS_SUPPORT) # temp.
    message(FATAL_ERROR "Macros are not yet available.")
endif()
option(KEXI_TABLE_PRINT_SUPPORT "Experimental: Enable printing of tabular view in Kexi" OFF) # broken since Kexi 2
if(KEXI_TABLE_PRINT_SUPPORT) # temp.
    message(FATAL_ERROR "Table printing is not yet available.")
endif()
option(KEXI_PROJECT_TEMPLATES "Experimental: Enable support for project templates in Kexi" OFF) # broken since Kexi 2
if(KEXI_PROJECT_TEMPLATES) # temp.
    message(FATAL_ERROR "Project templates are not yet available.")
endif()

#See commit 1e433a54cd9, left here for reference
#option(KEXI_SQLITE_MIGRATION "If defined, SQLite3 migration to some newer format is possible. Users can see a suitable question on app's startup." OFF)

add_definitions(-DTRANSLATION_DOMAIN=\"kexi\")

set(KEXI_PLUGIN_INSTALL_DIR ${PLUGIN_INSTALL_DIR}/kexi)
set(KEXI_FORM_WIDGETS_PLUGIN_INSTALL_DIR ${PLUGIN_INSTALL_DIR}/kexi/forms/widgets)


#no default: add_definitions(-DKDE_DEFAULT_DEBUG_AREA=44010)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config-kexi.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-kexi.h )

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${KOMAIN_INCLUDES}
  ${CMAKE_SOURCE_DIR}/libs
)

add_subdirectory( kexiutils )

macro_optional_find_package(MySQL)
macro_log_feature(MYSQL_FOUND "libmysqlclient" "MySQL Client Library" "http://www.mysql.com" FALSE "" "Required by Kexi MySQL migration driver")

macro_optional_find_package(CalligraPostgreSQL)
macro_log_feature(POSTGRESQL_FOUND "libpq" "C application programmer's interface to PostgreSQL" "http://www.postgresql.org" FALSE "" "Required by Kexi PostgreSQL migration driver")

macro_optional_find_package(FreeTDS)
macro_log_feature(FREETDS_FOUND "FreeTDS" "Open source implementation of the TDS (Tabular Data Stream) protocol" "http://www.freetds.org" FALSE "" "Required by Kexi Sybase migration driver")

macro_optional_find_package(XBase)
macro_log_feature(XBASE_FOUND "XBase" "XBase compatible C++ class library" "http://linux.techass.com/projects/xdb" FALSE "" "Required by Kexi XBase migration driver")

add_subdirectory( core )
add_subdirectory( widget )
add_subdirectory( data )
add_subdirectory( pics )
add_subdirectory( plugins )
if (BUILD_TESTING)
#TODO KEXI3 add_subdirectory( tests )
endif()

if(KEXI_MOBILE)

else()
  add_subdirectory( main )
  add_subdirectory( formeditor )
#TODO KEXI3  add_subdirectory( migration )
endif()

########### next target ###############

if(KEXI_MOBILE)
  add_subdirectory( mobile )
else()
  set(kexi_SRCS main.cpp )
  file(GLOB ICONS_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/pics/app/*-apps-calligrakexi.png")
  ecm_add_app_icon(kexi_SRCS ICONS ${ICONS_SRCS})
  add_executable(kexi ${kexi_SRCS})
  target_link_libraries(kexi

    keximain
    kexicore

    KDb
  )
  install(TARGETS kexi ${INSTALL_TARGETS_DEFAULT_ARGS})
endif()

########### install files ###############

install(PROGRAMS kexi.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
install(FILES kexi.appdata.xml DESTINATION ${SHARE_INSTALL_PREFIX}/appdata/)
