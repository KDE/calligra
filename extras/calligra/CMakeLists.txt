include_directories( ${KOMAIN_INCLUDES})

set(calligra_SRCS
    KoServiceLocator.cpp
    main.cpp
)

add_executable(calligra ${calligra_SRCS})

target_link_libraries(calligra komain KF5::KDELibs4Support)

install(TARGETS calligra ${INSTALL_TARGETS_DEFAULT_ARGS})
