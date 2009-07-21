set(SHAPES_SOURCES
    ${CMAKE_SOURCE_DIR}/plugins/pathshapes/ellipse/KoEllipseShape.cpp 
    ${CMAKE_SOURCE_DIR}/plugins/pathshapes/rectangle/KoRectangleShape.cpp
    ${CMAKE_SOURCE_DIR}/plugins/pathshapes/star/KoStarShape.cpp
    ${CMAKE_SOURCE_DIR}/plugins/pictureshape/PictureShape.cpp
    ${CMAKE_SOURCE_DIR}/plugins/artistictextshape/ArtisticTextShape.cpp
    ${CMAKE_SOURCE_DIR}/plugins/artistictextshape/ArtisticTextShapeLoadingUpdater.cpp)


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

add_subdirectory( png )
add_subdirectory( svg )
if (WPD_FOUND AND LIBWPG_FOUND)
  add_subdirectory( wpg )
endif (WPD_FOUND AND LIBWPG_FOUND)
add_subdirectory( wmf )
add_subdirectory( karbon1.x )
add_subdirectory( eps )

# enable again when these filters are ported
message(STATUS "Karbon: still need to port: pdf, ai, kontour, xcf, msod and oodraw filters")

if(FALSE)
add_subdirectory( pdf )
add_subdirectory( ai )
add_subdirectory( kontour )
add_subdirectory( xcf )
add_subdirectory( msod )
add_subdirectory( oodraw )
endif(FALSE)


