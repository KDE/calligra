add_definitions(-DTRANSLATION_DOMAIN=\"KarbonFilterEffects\")

include_directories( ${KOMAIN_INCLUDES} ${FLAKE_INCLUDES} )

set(calligra_filtereffects_PART_SRCS
    FilterEffectsPlugin.cpp
    BlurEffect.cpp
    BlurEffectFactory.cpp
    BlurEffectConfigWidget.cpp
    OffsetEffect.cpp
    OffsetEffectFactory.cpp
    OffsetEffectConfigWidget.cpp
    MergeEffect.cpp
    MergeEffectFactory.cpp
    MergeEffectConfigWidget.cpp
    ColorMatrixEffect.cpp
    ColorMatrixEffectFactory.cpp
    ColorMatrixEffectConfigWidget.cpp
    FloodEffect.cpp
    FloodEffectFactory.cpp
    FloodEffectConfigWidget.cpp
    CompositeEffect.cpp
    CompositeEffectFactory.cpp
    CompositeEffectConfigWidget.cpp
    BlendEffect.cpp
    BlendEffectFactory.cpp
    BlendEffectConfigWidget.cpp
    ComponentTransferEffect.cpp
    ComponentTransferEffectFactory.cpp
    ComponentTransferEffectConfigWidget.cpp
    ImageEffect.cpp
    ImageEffectFactory.cpp
    ImageEffectConfigWidget.cpp
    MorphologyEffect.cpp
    MorphologyEffectFactory.cpp
    MorphologyEffectConfigWidget.cpp
    ConvolveMatrixEffect.cpp
    ConvolveMatrixEffectFactory.cpp
    ConvolveMatrixEffectConfigWidget.cpp
    MatrixDataModel.cpp
    )

add_library(calligra_filtereffects MODULE ${calligra_filtereffects_PART_SRCS})


target_link_libraries(calligra_filtereffects flake kowidgets)

install(TARGETS calligra_filtereffects  DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/shapefiltereffects)
