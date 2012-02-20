add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/.. ${CMAKE_CURRENT_BINARY_DIR})

set(kritatouch_SRCS
    kto_canvas.cpp
    kto_icon_item.cpp
    kto_icon_provider.cpp
    kto_main_window.cpp
    kto_resource_information.cpp
    kto_resource_information_list.cpp
    main.cpp
)

kde4_add_executable(kritatouch ${kritatouch_SRCS})

target_link_libraries(kritatouch
    ${QT_QTDECLARATIVE_LIBRARY}
    ${QT_LIBRARIES}
    komain
    kritaui
    kowidgets
)

install(TARGETS kritatouch ${INSTALL_TARGETS_DEFAULT_ARGS})
install(PROGRAMS  kritatouch.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR})
install(DIRECTORY qml DESTINATION ${DATA_INSTALL_DIR}/kritatouch)
