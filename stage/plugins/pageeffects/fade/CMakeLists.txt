include_directories( ${KOMAIN_INCLUDES} ${FLAKE_INCLUDES}  ${CMAKE_SOURCE_DIR}/stage/part ${CMAKE_SOURCE_DIR}/stage/part/pageeffects )

set(kpr_pageeffect_fade_PART_SRCS Plugin.cpp
    KPrFadeEffectFactory.cpp
    KPrFadeCrossStrategy.cpp
    KPrFadeOverColorStrategy.cpp
)

add_library(kpr_pageeffect_fade MODULE ${kpr_pageeffect_fade_PART_SRCS})

target_link_libraries(
    kpr_pageeffect_fade
    calligrastageprivate
    flake

    Qt6::Core
    Qt6::Gui
)

install(TARGETS kpr_pageeffect_fade  DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligrastage/pageeffects)
