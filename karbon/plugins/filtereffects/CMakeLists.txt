include_directories( ${KOMAIN_INCLUDES} ${FLAKE_INCLUDES} )

set(karbonfiltereffects_PART_SRCS
    KarbonFilterEffectsPlugin.cpp
    BlurEffect.cpp
    BlurEffectFactory.cpp
    BlurEffectConfigWidget.cpp
    OffsetEffect.cpp
    OffsetEffectFactory.cpp
    OffsetEffectConfigWidget.cpp
)

kde4_add_plugin(karbonfiltereffects ${karbonfiltereffects_PART_SRCS})

target_link_libraries(
    karbonfiltereffects
    flake
    komain
    kostore
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KDEUI_LIBS}
    ${QT_QTCORE_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    )

install(TARGETS karbonfiltereffects  DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES karbonfiltereffects.desktop DESTINATION ${SERVICES_INSTALL_DIR} )
