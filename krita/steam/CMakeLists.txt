if(HAVE_STEAMWORKS)
    set(KRITASTEAM_INCLUDE_DIR steam PARENT_SCOPE)
    set(KRITA_STEAM_SRCS
        ../steam/kis_remotestorage.cpp
        ../steam/kis_steamcloudstorage.cpp
        ../steam/kritasteamclient.cpp
        PARENT_SCOPE)

set(kritasteamplugin_SRCS
    kritasteamplugin.cpp
    KisSteamComponent.cpp
    models/storagelistmodel.cpp
)

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})

include_directories(${KDE4_INCLUDES}
                    ${CMAKE_SOURCE_DIR}
                    ${CMAKE_SOURCE_DIR}/krita
                    ${CMAKE_SOURCE_DIR}/krita/image
                    ${CMAKE_SOURCE_DIR}/krita/ui
                    ${CMAKE_SOURCE_DIR}/krita/ui/canvas
                    ${CMAKE_SOURCE_DIR}/libs
                    ${CMAKE_SOURCE_DIR}/libs/flake
                    ${CMAKE_SOURCE_DIR}/libs/kundo2
                    ${CMAKE_SOURCE_DIR}/libs/main
                    ${CMAKE_SOURCE_DIR}/libs/pigment
                    ${CMAKE_SOURCE_DIR}/libs/odf
                    ${CMAKE_SOURCE_DIR}/libs/widgets
                    ${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}/..
                    ${CMAKE_CURRENT_SOURCE_DIR}/models
                    ${CMAKE_CURRENT_SOURCE_DIR}/../steam/models
                    ${CMAKE_CURRENT_BINARY_DIR})

# Sketch Qt Quick components
# =============================================================================

automoc4_add_library(kritasteamplugin MODULE ${kritasteamplugin_SRCS})

target_link_libraries(kritasteamplugin
    ${QT_QTDECLARATIVE_LIBRARY}
    ${QT_QTGUI_LIBRARIES}
    ${QT_QTCORE_LIBRARIES}
    ${QT_LIBRARIES}
    komain
    kritaui
    kowidgets
    ${KDE4_KFILE_LIBRARY}
    ${GLEW_LIBRARIES}
    kritasketchlib
)

include(GNUInstallDirs)

install(TARGETS   kritasteamplugin DESTINATION ${CMAKE_INSTALL_LIBDIR}/calligra/imports/org/krita/steam)
install(FILES
        qmldir
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/calligra/imports/org/krita/steam)


install(FILES
        qml/steamdialog.qml
        qml/SteamListItem.qml
        qml/SteamOpenFilePage.qml
        qml/SteamSaveAsFilePage.qml
        qml/SteamStoragePage.qml
        qml/SteamStorePage.qml
        qml/SteamTransfersPage.qml
        qml/SteamWorkshopPage.qml
        DESTINATION ${DATA_INSTALL_DIR}/krita/steam)

endif(HAVE_STEAMWORKS)
