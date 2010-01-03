include_directories( ${KOMAIN_INCLUDES} ${FLAKE_INCLUDES} )

set(karbonfiltereffects_PART_SRCS
    KarbonFilterEffectsPlugin.cpp
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
    )

kde4_add_plugin(karbonfiltereffects ${karbonfiltereffects_PART_SRCS})

target_link_libraries(karbonfiltereffects flake kowidgets)

install(TARGETS karbonfiltereffects  DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES karbonfiltereffects.desktop DESTINATION ${SERVICES_INSTALL_DIR} )
