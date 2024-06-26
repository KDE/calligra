project( artistictextshape )

add_definitions(-DTRANSLATION_DOMAIN=\"calligra_shape_artistictext\")

include_directories(
    ${KOWIDGETS_INCLUDES}
    ${FLAKE_INCLUDES}
)

set ( ArtisticTextShape_SRCS
    ArtisticTextShapePlugin.cpp
    ArtisticTextShape.cpp
    ArtisticTextRange.cpp
    ArtisticTextShapeFactory.cpp
    ArtisticTextTool.cpp
    ArtisticTextToolFactory.cpp
    ArtisticTextToolSelection.cpp
    ArtisticTextShapeConfigWidget.cpp
    ArtisticTextShapeOnPathWidget.cpp
    ArtisticTextShapeLoadingUpdater.cpp
    ArtisticTextLoadingContext.cpp
    AttachTextToPathCommand.cpp
    DetachTextFromPathCommand.cpp
    ChangeTextOffsetCommand.cpp
    ChangeTextFontCommand.cpp
    ChangeTextAnchorCommand.cpp
    AddTextRangeCommand.cpp
    RemoveTextRangeCommand.cpp
    MoveStartOffsetStrategy.cpp
    SelectTextStrategy.cpp
    ReplaceTextRangeCommand.cpp
)

ki18n_wrap_ui( ArtisticTextShape_SRCS
    ArtisticTextShapeConfigWidget.ui
    ArtisticTextShapeOnPathWidget.ui
)

add_library(calligra_shape_artistictext MODULE ${ArtisticTextShape_SRCS} )


target_link_libraries(calligra_shape_artistictext flake kowidgets)

install( TARGETS calligra_shape_artistictext DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/shapes )
