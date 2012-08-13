add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}/..
                    ${KRITA_INCLUDES}
                    ${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(draganddrop)

set(kritasketch_SRCS
    Constants.cpp
    Settings.cpp
    main.cpp
    MainWindow.cpp
    KisSketchView.cpp
    CanvasControllerDeclarative.cpp
    DocumentListModel.cpp
    IconImageProvider.cpp
    ProgressProxy.cpp
)

kde4_add_executable(kritasketch ${kritasketch_SRCS})

target_link_libraries(kritasketch
    ${QT_QTDECLARATIVE_LIBRARY}
    ${QT_LIBRARIES}
    komain
    kritaui
    kowidgets
    ${GLEW_LIBRARY}
)

install(TARGETS kritasketch ${INSTALL_TARGETS_DEFAULT_ARGS})
install(PROGRAMS  kritasketch.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR})
install(DIRECTORY qml DESTINATION ${DATA_INSTALL_DIR}/kritasketch)
install( FILES kritasketchrc DESTINATION ${CONFIG_INSTALL_DIR} )

