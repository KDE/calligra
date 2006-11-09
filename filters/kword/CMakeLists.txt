

macro_optional_find_package(WPD)
macro_optional_find_package(WV2)

include_directories(${CMAKE_SOURCE_DIR}/filters/kword/libexport ${KOFFICECORE_INCLUDES} )

add_subdirectory( libexport )
add_subdirectory( abiword )
add_subdirectory( amipro )
add_subdirectory( applixword )
add_subdirectory( ascii )
add_subdirectory( docbook )
add_subdirectory( html )
add_subdirectory( kword1.3 )
add_subdirectory( hancomword )
message(STATUS "When latex is reactivated (if?) also port it to GraphicsMagick, to make life easy for Isaac.")
#add_subdirectory( latex )
add_subdirectory( mswrite )
add_subdirectory( oowriter )
add_subdirectory( palmdoc )
# add_subdirectory( pdf )
message(STATUS "kword pdf import filter is disabled and should be replaced with a poppler based one")
add_subdirectory( rtf )
add_subdirectory( wml )

if(WPD_FOUND)
	add_subdirectory( wordperfect )
endif(WPD_FOUND)

if(WV2_FOUND)
	add_subdirectory(msword)
endif(WV2_FOUND)

