add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/.. ${CMAKE_CURRENT_BINARY_DIR})

set (kritatouch_RCCS KritaTouch.qrc)
qt4_add_resources(kritatouch_RCC_SRCS ${kritatouch_RCCS})

set(kritatouch_SRCS
    kto_canvas.cpp
    kto_main_window.cpp
    main.cpp
    ${kritatouch_RCCS}
)

kde4_add_executable(kritatouch ${kritatouch_SRCS})

target_link_libraries(kritatouch
    ${QT_QTDECLARATIVE_LIBRARY}
    ${QT_LIBRARIES}
    komain
    kritaui
)

install(TARGETS kritatouch ${INSTALL_TARGETS_DEFAULT_ARGS})
install(PROGRAMS  kritatouch.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR})
install(DIRECTORY qml DESTINATION ${DATA_INSTALL_DIR}/kritatouch)
