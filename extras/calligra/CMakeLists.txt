include_directories( ${KOMAIN_INCLUDES})

set(calligra_SRCS
    main.cpp
)

add_executable(calligra ${calligra_SRCS})

target_link_libraries(calligra komain KF5::Service KF5::KIOWidgets KF5::WidgetsAddons)

install(TARGETS calligra ${INSTALL_TARGETS_DEFAULT_ARGS})
