add_definitions(-DTRANSLATION_DOMAIN=\"calligra_shape_comment\")

include_directories(
    ${KOTEXT_INCLUDES}
    ${TEXTLAYOUT_INCLUDES}
    ${FLAKE_INCLUDES}
)

set( commentshape_LIB_SRCS
     CommentShape.cpp
     CommentShapeFactory.cpp
     CommentTool.cpp
     CommentToolFactory.cpp
     InitialsCommentShape.cpp
     Plugin.cpp
   )

add_library(calligra_shape_comment MODULE ${commentshape_LIB_SRCS})

target_link_libraries(calligra_shape_comment kotextlayout)

install(TARGETS calligra_shape_comment DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/shapes)
