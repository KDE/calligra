add_definitions(-DKDE_DEFAULT_DEBUG_AREA=44021)

add_subdirectory( tables )
add_subdirectory( queries )
add_subdirectory( forms )
add_subdirectory( migration )
add_subdirectory( importexport )

if(BUILD_koreport AND SHOULD_BUILD_KOREPORT)
    add_subdirectory( reports )
else(BUILD_koreport AND SHOULD_BUILD_KOREPORT)
    message(STATUS "WARNING: Kexi reports plugin will not be built because KoReport component is not available.")
endif(BUILD_koreport AND SHOULD_BUILD_KOREPORT)

if(SHOULD_BUILD_SCRIPTING)
    add_subdirectory(scripting)
endif(SHOULD_BUILD_SCRIPTING)

#TODO add_subdirectory( relations )
#TODO add_subdirectory(macros)
