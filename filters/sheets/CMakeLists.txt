include_directories(
    ${KOTEXT_INCLUDES}
)

add_subdirectory( applixspread )
add_subdirectory( csv )
add_subdirectory( dbase )
add_subdirectory( gnumeric )
add_subdirectory( opencalc )
add_subdirectory( qpro )

if(NOT WIN32 AND NOT QT_MAC_USE_COCOA)
    add_subdirectory( html )
    add_subdirectory( latex )
endif(NOT WIN32 AND NOT QT_MAC_USE_COCOA)

add_subdirectory( excel )
if(SHOULD_BUILD_OOXML_FILTERS)
    add_subdirectory( xlsx )
endif(SHOULD_BUILD_OOXML_FILTERS)
