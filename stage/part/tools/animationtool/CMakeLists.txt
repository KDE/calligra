
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

kcoreaddons_desktop_to_json(calligrastagetoolanimation calligrastagetoolanimation.desktop)

target_link_libraries(
    calligrastagetoolanimation
    calligrastageprivate
    KF5::IconThemes
)

install(TARGETS calligrastagetoolanimation  DESTINATION ${PLUGIN_INSTALL_DIR}/calligrastage/tools)


########### install files ###############

ecm_install_icons( ICONS
    16-actions-animation-stage.png
    DESTINATION ${DATA_INSTALL_DIR}/calligra/icons
    THEME hicolor
)

