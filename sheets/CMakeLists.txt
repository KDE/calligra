project(calligra-sheets)

list(APPEND CMAKE_AUTOMOC_MACRO_NAMES "CALLIGRA_SHEETS_EXPORT_FUNCTION_MODULE")

if (SHOULD_BUILD_PART_SHEETS)

include_directories( ${KOMAIN_INCLUDES}
                    ${KOTEXT_INCLUDES}
                    ${TEXTLAYOUT_INCLUDES}
                    ${Boost_INCLUDE_DIR} )

#add_definitions(-DCALLIGRA_SHEETS_MT)

add_subdirectory( engine )
add_subdirectory( functions )
add_subdirectory( core )
add_subdirectory( ui )
add_subdirectory( part )

# have their own translation domain
### TODO: The tableshape needs a rewrite,
### it has been obsoleted by the implementation of texttables
# The tableshape also registers the same actions as the main app, which causes issues.
# add_subdirectory( shape )

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





# TODO: with the new embedded JSON data for plugins there is no schema ATM to define extended
# propertiessheets_plugin.desktop, sheets_viewplugin.desktop

