project(calligra-stage)

add_definitions(-DTRANSLATION_DOMAIN=\"calligrastage\")

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
endif ()

if (SHOULD_BUILD_APP_STAGE)
    add_subdirectory( app )
endif ()
