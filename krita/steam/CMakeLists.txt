if(HAVE_STEAMWORKS)
    set(KRITASTEAM_INCLUDE_DIR steam PARENT_SCOPE)
    set(KRITA_STEAM_SRCS
        ../steam/kis_remotestorage.cpp
        ../steam/kis_steamcloudstorage.cpp
        ../steam/kritasteamclient.cpp
        PARENT_SCOPE)

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})

include_directories(${KDE4_INCLUDES}
                    ${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}/..
                    ${CMAKE_CURRENT_SOURCE_DIR}/models
                    ${CMAKE_CURRENT_SOURCE_DIR}/../steam/models
                    ${CMAKE_CURRENT_BINARY_DIR})
endif(HAVE_STEAMWORKS)
