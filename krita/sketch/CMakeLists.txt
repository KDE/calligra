add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})

include_directories(${CMAKE_CURRENT_SOURCE_DIR} 
                    ${CMAKE_CURRENT_SOURCE_DIR}/.. 
                    ${KRITA_INCLUDES}
                    ${CMAKE_CURRENT_BINARY_DIR})

set(kritasketch_SRCS
    constants.cpp
    settings.cpp
    main.cpp
    mainwindow.cpp
)
qt4_add_resources(kritasketch_SRCS krita-sketch.qrc)
kde4_add_executable(kritasketch ${kritasketch_SRCS})

target_link_libraries(kritasketch
    ${QT_QTDECLARATIVE_LIBRARY}
    ${QT_LIBRARIES}
    komain
    kritaui
    kowidgets
)

install(TARGETS kritasketch ${INSTALL_TARGETS_DEFAULT_ARGS})
install(PROGRAMS  kritasketch.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR})
install(DIRECTORY qml DESTINATION ${DATA_INSTALL_DIR}/kritasketch)
