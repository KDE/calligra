project(calligra-stage)

add_subdirectory( part )

if (SHOULD_BUILD_STAGE_APP)
    add_subdirectory( app )
endif (SHOULD_BUILD_STAGE_APP)

include_directories(
		${KOTEXT_INCLUDES}
        ${KOMAIN_INCLUDES}
)

add_subdirectory( pics )
add_subdirectory( templates )
add_subdirectory( plugins )
add_subdirectory( data )
add_subdirectory( styles )
