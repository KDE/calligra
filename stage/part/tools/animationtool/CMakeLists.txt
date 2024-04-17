
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
    KF6::IconThemes
)

install(TARGETS calligrastagetoolanimation  DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligrastage/tools)


########### install files ###############

ecm_install_icons( ICONS
    16-actions-animation-stage.png
    DESTINATION ${KDE_INSTALL_DATADIR}/calligra/icons
    THEME hicolor
)

