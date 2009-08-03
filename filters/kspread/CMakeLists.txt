include_directories(
    ${KOTEXT_INCLUDES}
)

add_subdirectory( applixspread )
add_subdirectory( csv )
add_subdirectory( dbase )
add_subdirectory( gnumeric )
add_subdirectory( opencalc )
add_subdirectory( qpro )
add_subdirectory( excel )

if(NOT WIN32)
    add_subdirectory( html )
    add_subdirectory( latex )
endif(NOT WIN32)

