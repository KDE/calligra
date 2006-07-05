project(kwordpart)

add_subdirectory( frame/tests )

include_directories( ${KOTEXT_INCLUDES} ${KOFFICEUI_INCLUDES} )

########### KWordPrivate library ###############

set(kwordprivate_LIB_SRCS
    KWFactory.cpp
    KWCanvas.cpp
    KWDocument.cpp
    KWGui.cpp
    KWView.cpp
    KWPage.cpp
    KWPageManager.cpp
    KWPageSettings.cpp
    KWViewMode.cpp
    KWViewModeNormal.cpp
    KWViewModePreview.cpp

    frame/KWTextDocumentLayout.cpp
    frame/KWFrameLayout.cpp
    frame/KWFrame.cpp
    frame/KWFrameSet.cpp
    frame/KWTextFrame.cpp
    frame/KWTextFrameSet.cpp

    dialog/KWFrameConnectSelector.cpp
    dialog/KWFrameGeometry.cpp
    dialog/KWFrameRunaroundProperties.cpp
    dialog/KWGeneralFrameProperties.cpp
    dialog/KWFrameDialog.cpp
    dialog/KWShapeConfigFactory.cpp
)

kde4_add_ui_files(kwordprivate_LIB_SRCS
    dialog/KWFrameConnectSelector.ui
    dialog/KWFrameGeometry.ui
    dialog/KWFrameRunaroundProperties.ui
    dialog/KWGeneralFrameProperties.ui
)

kde4_automoc(${kwordprivate_LIB_SRCS})

kde4_add_library(kwordprivate SHARED ${kwordprivate_LIB_SRCS})

target_link_libraries(kwordprivate  ${KDE4_KDECORE_LIBS} flake kofficeui kotext)

if(KDE4_BUILD_TESTS)
    # only with this definition will the KWORD_EXPORT_TEST macro do something
    add_definitions(-DCOMPILING_TESTS)
endif(KDE4_BUILD_TESTS)

set_target_properties(kwordprivate PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION} )
install(TARGETS kwordprivate  DESTINATION ${LIB_INSTALL_DIR})

########### KWordPart Library ###############

set(kwordpart_PART_SRCS KWFactoryInit.cpp )

kde4_automoc(${kwordpart_PART_SRCS})

kde4_add_plugin(kwordpart WITH_PREFIX ${kwordpart_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kwordpart )

target_link_libraries(kwordpart ${KDE4_KDECORE_LIBS} kwordprivate )

install(TARGETS kwordpart  DESTINATION ${PLUGIN_INSTALL_DIR})


########### KWord exe ###############

set(kword_KDEINIT_SRCS main.cpp )

kde4_automoc(${kword_KDEINIT_SRCS})

kde4_add_kdeinit_executable( kword ${kword_KDEINIT_SRCS})

target_link_libraries(kdeinit_kword  ${KDE4_KDECORE_LIBS} kofficecore )

install(TARGETS kdeinit_kword  DESTINATION ${LIB_INSTALL_DIR})

target_link_libraries( kword kdeinit_kword )
install(TARGETS kword  DESTINATION ${BIN_INSTALL_DIR})

########### install files ###############

install( FILES  kword.rc kword_readonly.rc  DESTINATION ${DATA_INSTALL_DIR}/kword)
install( FILES  kwordpart.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES  kword.desktop  DESTINATION ${XDG_APPS_DIR})
