project(calligra-stage)

include_directories(
    ${KOTEXT_INCLUDES}
    ${KOMAIN_INCLUDES}
)

if (SHOULD_BUILD_PART_STAGE)
    add_subdirectory( part )
    add_subdirectory( pics )
    add_subdirectory( templates )
    add_subdirectory( plugins )
    add_subdirectory( data )
    add_subdirectory( styles )
endif (SHOULD_BUILD_PART_STAGE)

if (SHOULD_BUILD_APP_STAGE)
    add_subdirectory( app )
endif (SHOULD_BUILD_APP_STAGE)
