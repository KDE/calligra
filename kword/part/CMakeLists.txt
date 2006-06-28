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
    frame/KWTextFrameSet.cpp)

kde4_automoc(${kwordprivate_LIB_SRCS})

kde4_add_library(kwordprivate SHARED ${kwordprivate_LIB_SRCS})

target_link_libraries(kwordprivate  ${KDE4_KDECORE_LIBS} flake kofficeui kotext)

if(KDE4_BUILD_TESTS)
    add_definitions(-DCOMPILING_TESTS)
endif(KDE4_BUILD_TESTS)

set_target_properties(kwordprivate PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION} )
install_targets(${LIB_INSTALL_DIR} kwordprivate )

########### KWordPart Library ###############

set(kwordpart_PART_SRCS KWFactoryInit.cpp )

kde4_automoc(${kwordpart_PART_SRCS})

kde4_add_plugin(kwordpart WITH_PREFIX ${kwordpart_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kwordpart )

target_link_libraries(kwordpart ${KDE4_KDECORE_LIBS} kwordprivate )

install_targets(${PLUGIN_INSTALL_DIR} kwordpart )


########### KWord exe ###############

set(kword_KDEINIT_SRCS main.cpp )

kde4_automoc(${kword_KDEINIT_SRCS})

kde4_add_kdeinit_executable( kword ${kword_KDEINIT_SRCS})

target_link_libraries(kdeinit_kword  ${KDE4_KDECORE_LIBS} kofficecore )

install_targets(${LIB_INSTALL_DIR} kdeinit_kword )

target_link_libraries( kword kdeinit_kword )
install_targets(/bin kword )

########### install files ###############

install_files( ${DATA_INSTALL_DIR}/kword FILES  kword.rc kword_readonly.rc )
install_files( ${SERVICES_INSTALL_DIR} FILES  kwordpart.desktop )
install_files( ${XDG_APPS_DIR} FILES  kword.desktop )
