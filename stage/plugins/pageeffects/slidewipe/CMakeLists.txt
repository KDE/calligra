include_directories( ${KOMAIN_INCLUDES} ${FLAKE_INCLUDES}  ${CMAKE_SOURCE_DIR}/stage/part ${CMAKE_SOURCE_DIR}/stage/part/pageeffects )

set(kpr_pageeffect_slidewipe_PART_SRCS Plugin.cpp
KPrSlideWipeEffectFactory.cpp
KPrSlideWipeFromBottomStrategy.cpp
KPrSlideWipeFromLeftStrategy.cpp
KPrSlideWipeFromRightStrategy.cpp
KPrSlideWipeFromTopStrategy.cpp
KPrSlideWipeToBottomStrategy.cpp
KPrSlideWipeToLeftStrategy.cpp
KPrSlideWipeToRightStrategy.cpp
KPrSlideWipeToTopStrategy.cpp
)


add_library(kpr_pageeffect_slidewipe MODULE ${kpr_pageeffect_slidewipe_PART_SRCS})

calligrastage_pageeffect_desktop_to_json(kpr_pageeffect_slidewipe kpr_pageeffect_slidewipe.desktop)

target_link_libraries(
    kpr_pageeffect_slidewipe
    calligrastageprivate
    flake
    
    
    Qt5::Core
    Qt5::Gui    
    )

install(TARGETS kpr_pageeffect_slidewipe  DESTINATION ${PLUGIN_INSTALL_DIR}/calligrastage/pageeffects)
