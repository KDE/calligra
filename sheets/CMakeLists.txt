project(calligra-sheets)

list(APPEND CMAKE_AUTOMOC_MACRO_NAMES "CALLIGRA_SHEETS_EXPORT_FUNCTION_MODULE")

if (SHOULD_BUILD_PART_SHEETS)

include_directories( ${CMAKE_SOURCE_DIR}/interfaces
                    ${KOMAIN_INCLUDES}
                    ${KOTEXT_INCLUDES}
                    ${TEXTLAYOUT_INCLUDES}
                    ${Boost_INCLUDE_DIR}
                    ${EIGEN3_INCLUDE_DIR} )

#add_definitions(-DCALLIGRA_SHEETS_MT)

add_subdirectory( engine )
add_subdirectory( functions )
add_subdirectory( core )
add_subdirectory( ui )
add_subdirectory( part )

# have their own translation domain
### TODO: The tableshape needs a rewrite,
### it has been obsoleted by the implementation of texttables
add_subdirectory( shape )

add_subdirectory( plugins )

add_definitions(-DTRANSLATION_DOMAIN=\"calligrasheets\")

add_subdirectory( data )
if(BUILD_TESTING)
    add_subdirectory( tests )
endif()
add_subdirectory( dtd )

endif (SHOULD_BUILD_PART_SHEETS)

########### APP ###############

if (SHOULD_BUILD_APP_SHEETS)

add_subdirectory( app )

endif ()























# TODO - all this needs to go somewhere!!!




########### next target ###############
set (chart_DIR_SRCS
    chart/ChartDatabaseSelectorFactory.cpp
    chart/ChartDatabaseSelector.cpp
    chart/ChartDialog.cpp
    )

ki18n_wrap_ui(chart_DIR_SRCS
    chart/ChartDatabaseSelector.ui
    )


if(Qt5DBus_FOUND)
set (interfaces_DIR_SRCS
    interfaces/MapAdaptor.cpp
    interfaces/SheetAdaptor.cpp
    interfaces/ViewAdaptor.cpp
    )
endif()


set (calligrasheetscommon_LIB_SRCS
    MapModel.cpp
    PageManager.cpp
    RegionModel.cpp
    ${chart_DIR_SRCS}
    ${commands_DIR_SRCS}
    ${dialogs_DIR_SRCS}
    ${part_DIR_SRCS}
    ${ui_DIR_SRCS}
    )

if(Qt5DBus_FOUND)
set (calligrasheetscommon_LIB_SRCS
    ${calligrasheetscommon_LIB_SRCS}
    ${interfaces_DIR_SRCS}
    )
endif()



########### install files ###############

# TODO: with the new embedded JSON data for plugins there is no schema ATM to define extended
# propertiessheets_plugin.desktop, sheets_viewplugin.desktop

