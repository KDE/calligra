set(SHAPES_SOURCES
    ${CMAKE_SOURCE_DIR}/shapes/pathshapes/ellipse/KoEllipseShape.cpp 
    ${CMAKE_SOURCE_DIR}/shapes/pathshapes/rectangle/KoRectangleShape.cpp
    ${CMAKE_SOURCE_DIR}/shapes/pathshapes/star/KoStarShape.cpp
    ${CMAKE_SOURCE_DIR}/shapes/pictureshape/PictureShape.cpp
    ${CMAKE_SOURCE_DIR}/shapes/text/TextShape.cpp
    ${CMAKE_SOURCE_DIR}/shapes/text/Layout.cpp
    ${CMAKE_SOURCE_DIR}/shapes/text/ListItemsHelper.cpp)


include_directories(
        ${KOTEXT_INCLUDES}
        ${KOMAIN_INCLUDES}
        ${FLAKE_INCLUDES}
        ${CMAKE_SOURCE_DIR}/karbon
        ${CMAKE_SOURCE_DIR}/karbon/core
        ${CMAKE_SOURCE_DIR}/karbon/visitors
        ${CMAKE_BINARY_DIR}/karbon
        ${KDE4_INCLUDES}
        ${CMAKE_SOURCE_DIR}/shapes/ )

add_subdirectory( png )
add_subdirectory( svg )
add_subdirectory( wpg )
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


