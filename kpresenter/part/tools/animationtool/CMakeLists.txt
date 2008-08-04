
include_directories( ../.. )

set(kpresentertoolanimation_PART_SRCS
    Plugin.cpp
    KPrAnimationTool.cpp
    KPrAnimationToolFactory.cpp
    KPrPageEffectDocker.cpp
    )


#kde4_add_ui_files(kpresentertoolanimation_PART_SRCS wdg_tool_crop.ui)

kde4_add_plugin(kpresentertoolanimation ${kpresentertoolanimation_PART_SRCS})



target_link_libraries(kpresentertoolanimation ${KDE4_KDECORE_LIBS} ${KDE4_PHONON_LIBRARY} koguiutils flake kpresenterprivate kopageapp)

install(TARGETS kpresentertoolanimation  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kpresentertoolanimation.desktop  DESTINATION ${SERVICES_INSTALL_DIR})


