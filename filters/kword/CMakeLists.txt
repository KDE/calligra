

 macro_optional_find_package(WV2)
macro_log_feature(WV2_FOUND "wvWare" "Microsoft Word .doc Library" "http://sourceforge.net/projects/wvware" FALSE "" "Required by the KWord .doc filter")

include_directories(${CMAKE_SOURCE_DIR}/filters/kword/libexport ${KOMAIN_INCLUDES} )

add_subdirectory( libexport )

if (NOT TINY)

    macro_optional_find_package(WPD)
    macro_log_feature(WPD_FOUND "WPD" "WordPerfect Document Library" "http://libwpd.sourceforge.net/" FALSE "" "Required by the KWord WPD import filter")
    
    add_subdirectory( abiword )
    add_subdirectory( amipro )
    add_subdirectory( applixword )
    add_subdirectory( docbook )

    #html FIXME: export dialog is Qt 3 version ui file
    if(NOT WIN32)
        add_subdirectory( html )
        if (TARGET kowmf)
            add_subdirectory( mswrite )
        endif (TARGET kowmf)
        add_subdirectory( kword1.3 )
        add_subdirectory( hancomword )
        message(STATUS "When latex is reactivated (if?) also port it to GraphicsMagick, to make life easy for Isaac.")
        #add_subdirectory( latex )
    endif(NOT WIN32)

    add_subdirectory( oowriter )
    add_subdirectory( palmdoc )
    add_subdirectory( wml )

    if(WPD_FOUND)
    	add_subdirectory( wordperfect )
    endif(WPD_FOUND)
    
endif (NOT TINY)

add_subdirectory( ascii )
add_subdirectory( rtf )
if(WV2_FOUND)
	add_subdirectory(msword-odf)
endif(WV2_FOUND)

add_subdirectory( docx )
