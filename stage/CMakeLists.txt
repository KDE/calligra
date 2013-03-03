project(calligra-stage)

include_directories(
    ${KOTEXT_INCLUDES}
    ${KOMAIN_INCLUDES}
)

if (SHOULD_BUILD_STAGE_PART)
    add_subdirectory( part )
    add_subdirectory( pics )
    add_subdirectory( templates )
    add_subdirectory( plugins )
    add_subdirectory( data )
    add_subdirectory( styles )
endif (SHOULD_BUILD_STAGE_PART)

if (SHOULD_BUILD_STAGE_APP)
    add_subdirectory( app )
endif (SHOULD_BUILD_STAGE_APP)

