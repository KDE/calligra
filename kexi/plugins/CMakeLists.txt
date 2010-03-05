add_definitions(-DKDE_DEFAULT_DEBUG_AREA=44021)

add_subdirectory( tables )
add_subdirectory( queries )
add_subdirectory( forms )
add_subdirectory( migration )
add_subdirectory( importexport )

if(BUILD_kchart)
    add_subdirectory( reports )
endif(BUILD_kchart)

if(SHOULD_BUILD_SCRIPTING)
    add_subdirectory(scripting)
endif(SHOULD_BUILD_SCRIPTING)

#TODO add_subdirectory( relations )
#TODO add_subdirectory(macros)
