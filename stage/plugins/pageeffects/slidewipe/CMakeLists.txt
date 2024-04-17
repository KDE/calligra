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

target_link_libraries(
    kpr_pageeffect_slidewipe
    calligrastageprivate
    flake
    Qt6::Core
    Qt6::Gui
)

install(TARGETS kpr_pageeffect_slidewipe  DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligrastage/pageeffects)
