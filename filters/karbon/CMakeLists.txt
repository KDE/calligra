
include_directories(
        ${KOTEXT_INCLUDES}
        ${KOMAIN_INCLUDES}
        ${FLAKE_INCLUDES}
        ${CMAKE_SOURCE_DIR}/karbon
        ${CMAKE_SOURCE_DIR}/karbon/common
        ${CMAKE_SOURCE_DIR}/karbon/ui
        ${CMAKE_BINARY_DIR}/karbon
        ${KDE4_INCLUDES}
        ${CMAKE_SOURCE_DIR}/plugins/ )

macro_optional_find_package(WPD)
macro_optional_find_package(WPG)
macro_log_feature(LIBWPG_FOUND "WPG" "WordPerfect Graphics Library" "http://libwpg.sourceforge.net/" FALSE "" "Required by the Karbon WPG import filter")
macro_optional_find_package(Poppler)
# The pdf-importer needs the not-officially-supported XPDF Headers
# Installing these is off by default in poppler sources, so lets make
# sure they're really there before trying to build the pdf import
find_path(POPPLER_XPDF_HEADERS poppler-config.h
    HINTS ${POPPLER_INCLUDE_DIR} )
if( POPPLER_FOUND AND NOT POPPLER_XPDF_HEADERS )
    message( STATUS "Could NOT find the XPDF headers in Poppler Qt4 which are necessary for the Karbon PDF import filter")
    set( POPPLER_FOUND FALSE )
endif( POPPLER_FOUND AND NOT POPPLER_XPDF_HEADERS )
macro_log_feature( POPPLER_FOUND "poppler-qt4" "The Poppler Qt4 interface library, with XPDF headers" "http://poppler.freedesktop.org" FALSE "" "Required by the Karbon PDF import filter")

add_subdirectory( image )
add_subdirectory( svg )
if (WPD_FOUND AND LIBWPG_FOUND)
  add_subdirectory( wpg )
endif (WPD_FOUND AND LIBWPG_FOUND)
add_subdirectory( wmf )
add_subdirectory( karbon1.x )
if (QT3SUPPORT)
    add_subdirectory( eps )
endif (QT3SUPPORT)
if (POPPLER_FOUND AND NOT WIN32)
    #add_subdirectory( pdf )
endif (POPPLER_FOUND AND NOT WIN32)
#add_subdirectory( xfig )

