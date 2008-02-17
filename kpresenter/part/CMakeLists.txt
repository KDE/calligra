project(kpresenter)

include_directories(${KOGUIUTILS_INCLUDES} ${KOTEXT_INCLUDES} ${KOPAGEAPP_INCLUDES} ${Boost_INCLUDE_DIR})

if(KDE4_BUILD_TESTS)
    # only with this definition will the KPRESENTER_TEST_EXPORT macro do something
    add_definitions(-DCOMPILING_TESTS)
endif(KDE4_BUILD_TESTS)


add_subdirectory(tests)

### kpresenterprivate ###
set( kpresenterprivate_LIB_SRCS
     KPrFactory.cpp
     KPrDocument.cpp
     KPrView.cpp
     KPrViewModePresentation.cpp
     KPrPresentationTool.cpp
     KPrAnimationDirector.cpp
     KPrShapeAnimations.cpp
     KPrShapeManagerAnimationStrategy.cpp
     KPrAnimationController.cpp
     KPrPage.cpp
     KPrMasterPage.cpp
     KPrPageApplicationData.cpp
     KPrShapeApplicationData.cpp
     KPrCustomSlideShows.cpp

     commands/KPrAnimationCreateCommand.cpp
     commands/KPrPageEffectSetCommand.cpp

     dockers/KPrPageEffectDockerFactory.cpp
     dockers/KPrPageEffectDocker.cpp

     pageeffects/KPrPageEffectRunner.cpp
     pageeffects/KPrPageEffect.cpp
     pageeffects/KPrPageEffectStrategy.cpp
     pageeffects/KPrPageEffectFactory.cpp
     pageeffects/KPrPageEffectRegistry.cpp
     pageeffects/slidewipe/KPrSlideWipeEffectFactory.cpp
     pageeffects/slidewipe/KPrSlideWipeFromBottomStrategy.cpp
     pageeffects/slidewipe/KPrSlideWipeFromLeftStrategy.cpp
     pageeffects/slidewipe/KPrSlideWipeFromRightStrategy.cpp
     pageeffects/slidewipe/KPrSlideWipeFromTopStrategy.cpp
     pageeffects/slidewipe/KPrSlideWipeToBottomStrategy.cpp
     pageeffects/slidewipe/KPrSlideWipeToLeftStrategy.cpp
     pageeffects/slidewipe/KPrSlideWipeToRightStrategy.cpp
     pageeffects/slidewipe/KPrSlideWipeToTopStrategy.cpp
     pageeffects/barwipe/KPrBarWipeEffectFactory.cpp
     pageeffects/barwipe/KPrBarWipeFromLeftStrategy.cpp
     pageeffects/barwipe/KPrBarWipeFromTopStrategy.cpp

     shapeanimations/KPrShapeAnimation.cpp
     shapeanimations/KPrTranslateAnimation.cpp
     shapeanimations/KPrAnimationMoveAppear.cpp
     shapeanimations/KPrAnimationDisappear.cpp
     shapeanimations/KPrShapeAnimationFactory.cpp
     shapeanimations/KPrShapeAnimationRegistry.cpp

     ui/KPrCustomSlideShowsDialog.cpp
   )

kde4_add_ui_files(kpresenterprivate_LIB_SRCS
     ui/KPrCustomSlideShowsDialog.ui
   )

kde4_add_library(kpresenterprivate SHARED ${kpresenterprivate_LIB_SRCS})

target_link_libraries(kpresenterprivate ${KDE4_KDECORE_LIBS} koguiutils flake kotext kopageapp)

set_target_properties(kpresenterprivate PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION} )
install(TARGETS kpresenterprivate  DESTINATION ${LIB_INSTALL_DIR})

### kpresenterpart ###
set(kpresenterpart_PART_SRCS KPrFactoryInit.cpp )

kde4_add_plugin(kpresenterpart WITH_PREFIX ${kpresenterpart_PART_SRCS})



target_link_libraries(kpresenterpart  ${KDE4_KDECORE_LIBS} kpresenterprivate )

install(TARGETS kpresenterpart DESTINATION ${PLUGIN_INSTALL_DIR})

### kdeinit kpresenter ###
set(kpresenter_KDEINIT_SRCS main.cpp )

kde4_add_kdeinit_executable( kpresenter ${kpresenter_KDEINIT_SRCS})

target_link_libraries(kdeinit_kpresenter  ${KDE4_KDECORE_LIBS} komain )

install(TARGETS kdeinit_kpresenter DESTINATION ${LIB_INSTALL_DIR})

target_link_libraries(kpresenter kdeinit_kpresenter)
install(TARGETS kpresenter DESTINATION ${BIN_INSTALL_DIR})

### desktop files ###
install( FILES kpresenterpart.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES kpresenter.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})

### GUI files ###
install( FILES kpresenter.rc kpresenter_readonly.rc DESTINATION ${DATA_INSTALL_DIR}/kpresenter)
