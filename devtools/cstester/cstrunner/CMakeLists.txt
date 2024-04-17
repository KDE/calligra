include_directories(
    
)

set(cstrunner_SRCS
    CSTProcessRunner.cpp
    cstrunner.cpp)

add_executable(cstrunner ${cstrunner_SRCS})

target_link_libraries(cstrunner Qt6::Core Qt6::Gui)

install(TARGETS cstrunner ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

