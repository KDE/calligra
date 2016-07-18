project(words)

add_definitions(-DTRANSLATION_DOMAIN=\"calligrawords\")

include_directories(
    ${KOMAIN_INCLUDES}
)

if (SHOULD_BUILD_PART_WORDS)
    add_subdirectory( templates )
    add_subdirectory( pics )
    add_subdirectory( part )
    add_subdirectory( plugins )
    add_subdirectory( styles )
endif ()

if (SHOULD_BUILD_APP_WORDS)
    add_subdirectory( app )
endif ()
