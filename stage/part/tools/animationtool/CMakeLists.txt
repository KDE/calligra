
include_directories( ../.. )

set(calligrastagetoolanimation_PART_SRCS
    Plugin.cpp
    KPrAnimationTool.cpp
    KPrAnimationToolFactory.cpp
    KPrPageEffectDocker.cpp
    KPrClickActionDocker.cpp
    KPrShapeAnimationDocker.cpp
    KPrEditAnimationsWidget.cpp
    KPrCollectionItemModel.cpp
    KPrAnimationGroupProxyModel.cpp
    KPrAnimationSelectorWidget.cpp
    KPrPredefinedAnimationsLoader.cpp
    KPrAnimationsTimeLineView.cpp
    KPrTimeLineHeader.cpp
    KPrTimeLineView.cpp
    )


#kde4_add_ui_files(calligrastagetoolanimation_PART_SRCS wdg_tool_crop.ui)

kde4_add_plugin(calligrastagetoolanimation ${calligrastagetoolanimation_PART_SRCS})



target_link_libraries(
    calligrastagetoolanimation
    calligrastageprivate
    ${KDE4_PHONON_LIBRARY}
    )

install(TARGETS calligrastagetoolanimation  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  calligrastagetoolanimation.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

kde4_install_icons( ${DATA_INSTALL_DIR}/calligra/icons  )

