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

kde4_add_plugin(kpr_pageeffect_clockwipe ${kpr_pageeffect_clockwipe_SRCS})

target_link_libraries(
    kpr_pageeffect_clockwipe 
    calligrastageprivate 
    flake
    
    ${KDE4_KDECORE_LIBS}
    ${QT_QTCORE_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    )

install(TARGETS kpr_pageeffect_clockwipe  DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES kpr_pageeffect_clockwipe.desktop DESTINATION ${SERVICES_INSTALL_DIR})


