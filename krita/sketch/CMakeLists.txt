if (WIN32)
    option(ENABLE_SKETCH_STARTUP_MAGIC "Enable Sketch's windows startup magic" ON)

    if (ENABLE_SKETCH_STARTUP_MAGIC)
        add_definitions(-DSKETCH_ENABLE_STARTUP_MAGIC)
    endif ()
endif ()

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}/..
                    ${CMAKE_CURRENT_SOURCE_DIR}/models
                    ${CMAKE_CURRENT_BINARY_DIR})
)

set(kritasketch_SRCS
    SketchApplication.cpp
    main.cpp
    MainWindow.cpp
)

if (WIN32)
    set(kritasketch_SRCS ${kritasketch_SRCS} KritaSketchWin.rc)
else ()
    kde4_add_app_icon(kritasketch_SRCS "${KDE4_ICON_DIR}/oxygen/*/apps/kritasketch.png")
endif ()

# Krita Sketch application
# =============================================================================

kde4_add_executable(kritasketch ${kritasketch_SRCS})

target_link_libraries(kritasketch
    ${QT_QTDECLARATIVE_LIBRARY}
    ${QT_QTGUI_LIBRARIES}
    ${QT_QTCORE_LIBRARIES}
    ${QT_LIBRARIES}
    ${GLEW_LIBRARIES}
    kritaui
    kritasketchlib
)

install(TARGETS   kritasketch ${INSTALL_TARGETS_DEFAULT_ARGS})
install(PROGRAMS  kritasketch.desktop               DESTINATION ${XDG_APPS_INSTALL_DIR})
install(FILES     kritasketchrc kritasketchpanelsrc DESTINATION ${CONFIG_INSTALL_DIR} )
install(FILES     kritasketch.qml                   DESTINATION ${DATA_INSTALL_DIR}/kritasketch)
install(FILES     kritasketch.rc                    DESTINATION ${DATA_INSTALL_DIR}/kritasketch)

add_subdirectory(icon)
