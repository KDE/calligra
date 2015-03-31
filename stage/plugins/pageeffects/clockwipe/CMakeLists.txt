include_directories( ${KOMAIN_INCLUDES} ${FLAKE_INCLUDES}  ${CMAKE_SOURCE_DIR}/stage/part ${CMAKE_SOURCE_DIR}/stage/part/pageeffects )

set(kpr_pageeffect_clockwipe_SRCS
    Plugin.cpp
    KPrClockWipeStrategy.cpp
    KPrCenterFanWipeStrategy.cpp
    KPrSideFanWipeStrategy.cpp
    KPrClockWipeSubpathHelper.cpp
    KPrSweepWipeStrategy.cpp

    clockwipe/KPrClockWipeEffectFactory.cpp
    pinwheelwipe/KPrPinWheelWipeEffectFactory.cpp
    singlesweepwipe/KPrSingleSweepWipeEffectFactory.cpp
    singlesweepwipe/KPrSingleSweepWipeStrategy.cpp
    fanwipe/KPrFanWipeEffectFactory.cpp
    doublefanwipe/KPrDoubleFanWipeEffectFactory.cpp
    doublesweepwipe/KPrDoubleSweepWipeEffectFactory.cpp
    doublesweepwipe/KPrDoubleSweepWipeStrategy.cpp
    saloondoorwipe/KPrSaloonDoorWipeEffectFactory.cpp
    saloondoorwipe/KPrSaloonDoorWipeStrategy.cpp
    windshieldwipe/KPrWindShieldWipeEffectFactory.cpp
    windshieldwipe/KPrWindShieldWipeStrategy.cpp
    )

add_library(kpr_pageeffect_clockwipe MODULE ${kpr_pageeffect_clockwipe_SRCS})

kcoreaddons_desktop_to_json(kpr_pageeffect_clockwipe kpr_pageeffect_clockwipe.desktop)

target_link_libraries(
    kpr_pageeffect_clockwipe 
    calligrastageprivate 
    flake
    
    
    Qt5::Core
    Qt5::Gui
    )

install(TARGETS kpr_pageeffect_clockwipe  DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})
