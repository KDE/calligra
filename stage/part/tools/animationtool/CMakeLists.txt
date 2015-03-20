
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


#ki18n_wrap_ui(calligrastagetoolanimation_PART_SRCS wdg_tool_crop.ui)

add_library(calligrastagetoolanimation MODULE ${calligrastagetoolanimation_PART_SRCS})



target_link_libraries(
    calligrastagetoolanimation
    calligrastageprivate
    Phonon::phonon4qt5
    )

install(TARGETS calligrastagetoolanimation  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  calligrastagetoolanimation.desktop  DESTINATION ${SERVICES_INSTALL_DIR}/calligra)

ecm_install_icons( ${DATA_INSTALL_DIR}/calligra/icons  )

