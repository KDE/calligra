set(SHAPES_SOURCES
    ${CMAKE_SOURCE_DIR}/plugins/pathshapes/ellipse/KoEllipseShape.cpp 
    ${CMAKE_SOURCE_DIR}/plugins/pathshapes/rectangle/KoRectangleShape.cpp
    ${CMAKE_SOURCE_DIR}/plugins/pathshapes/star/KoStarShape.cpp
    ${CMAKE_SOURCE_DIR}/plugins/pictureshape/PictureShape.cpp
    ${CMAKE_SOURCE_DIR}/plugins/textshape/TextShape.cpp
    ${CMAKE_SOURCE_DIR}/plugins/textshape/Layout.cpp
    ${CMAKE_SOURCE_DIR}/plugins/textshape/ListItemsHelper.cpp
    ${CMAKE_SOURCE_DIR}/plugins/simpletextshape/SimpleTextShape.cpp)


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

macro_optional_find_package(WPG)

add_subdirectory( png )
add_subdirectory( svg )
if (LIBWPD AND LIBWPG_FOUND)
  add_subdirectory( wpg )
endif (LIBWPD AND LIBWPG_FOUND)
add_subdirectory( wmf )
add_subdirectory( karbon1.x )

# enable again when these filters are ported
if(FALSE)
add_subdirectory( eps )
add_subdirectory( ai )
add_subdirectory( kontour )
add_subdirectory( xcf )
add_subdirectory( msod )
add_subdirectory( oodraw )
endif(FALSE)


