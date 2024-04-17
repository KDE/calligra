include_directories( ${KOMAIN_INCLUDES})

set(calligra_SRCS
    main.cpp
)

add_executable(calligra ${calligra_SRCS})

target_link_libraries(calligra komain KF6::Service KF6::KIOWidgets KF6::WidgetsAddons)

install(TARGETS calligra ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
