include_directories( ${KOMAIN_INCLUDES} ${FLAKE_INCLUDES}  ${CMAKE_SOURCE_DIR}/stage/part ${CMAKE_SOURCE_DIR}/stage/part/pageeffects )

set(kpr_pageeffect_fade_PART_SRCS Plugin.cpp
    KPrFadeEffectFactory.cpp
    KPrFadeCrossStrategy.cpp
    KPrFadeOverColorStrategy.cpp
)

add_library(kpr_pageeffect_fade MODULE ${kpr_pageeffect_fade_PART_SRCS})

kcoreaddons_desktop_to_json(kpr_pageeffect_fade kpr_pageeffect_fade.desktop)

target_link_libraries(
    kpr_pageeffect_fade
    calligrastageprivate
    flake 
    
    
    Qt5::Core
    Qt5::Gui
    )

install(TARGETS kpr_pageeffect_fade  DESTINATION ${PLUGIN_INSTALL_DIR}/calligrastage/pageeffects)
