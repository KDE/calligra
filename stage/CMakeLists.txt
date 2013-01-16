project(calligra-stage)

add_subdirectory( part )

include_directories(
		${KOTEXT_INCLUDES}
        ${KOMAIN_INCLUDES}
)

add_subdirectory( pics )
add_subdirectory( templates )
add_subdirectory( plugins )
add_subdirectory( data )
add_subdirectory( styles )
