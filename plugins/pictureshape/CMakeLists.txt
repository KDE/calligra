add_definitions(-DTRANSLATION_DOMAIN=\"calligra_shape_picture\")

include_directories( ${FLAKE_INCLUDES} )

########### Flake Plugin library ###############

set (pictureshape_LIB_SRCS
    PictureDebug.cpp
    PictureShape.cpp
    PictureShapeFactory.cpp
    PictureTool.cpp
    PictureToolFactory.cpp
    Plugin.cpp
    PictureShapeConfigWidget.cpp
    ChangeImageCommand.cpp
    ClipCommand.cpp
    CropWidget.cpp
    SelectionRect.cpp
    ClippingRect.cpp
    filters/GreyscaleFilterEffect.cpp
    filters/MonoFilterEffect.cpp
    filters/WatermarkFilterEffect.cpp
    filters/ColoringFilterEffect.cpp
    filters/GammaFilterEffect.cpp
)

ki18n_wrap_ui(pictureshape_LIB_SRCS forms/wdgPictureTool.ui)
add_library(calligra_shape_picture MODULE ${pictureshape_LIB_SRCS})

target_link_libraries(calligra_shape_picture
    flake
    KF6::I18n
    KF6::KIOFileWidgets
    KF6::GuiAddons
)
install(TARGETS calligra_shape_picture DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/shapes)
