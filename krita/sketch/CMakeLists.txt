add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}/..
                    ${CMAKE_CURRENT_SOURCE_DIR}/models
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

set(kritasketch_kritaintegrationmodels_SRCS
    models/ColorImageProvider.cpp
    models/PaletteColorsModel.cpp
    models/PaletteModel.cpp
    models/PresetImageProvider.cpp
    models/PresetModel.cpp
)

kde4_add_executable(kritasketch ${kritasketch_SRCS} ${kritasketch_kritaintegrationmodels_SRCS})

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
