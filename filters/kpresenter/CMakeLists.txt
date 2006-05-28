include_directories(${CMAKE_SOURCE_DIR}/filters/kword/libexport ${KOFFICECORE_INCLUDES} ${KOFFICEUI_INCLUDES} ${KOTEXT_INCLUDES})

add_subdirectory( libimageexport )
add_subdirectory( png )
add_subdirectory( bmp )
add_subdirectory( xbm )
add_subdirectory( xpm )
add_subdirectory( mng )
add_subdirectory( svg )
add_subdirectory( jpeg )
add_subdirectory( kword )
add_subdirectory( ooimpress )
add_subdirectory( powerpoint )

