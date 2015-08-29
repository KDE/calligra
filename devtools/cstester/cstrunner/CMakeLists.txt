include_directories(
    
)

set(cstrunner_SRCS
    CSTProcessRunner.cpp
    cstrunner.cpp)

add_executable(cstrunner ${cstrunner_SRCS})

target_link_libraries(cstrunner Qt5::Core Qt5::Gui)

install(TARGETS cstrunner ${INSTALL_TARGETS_DEFAULT_ARGS})

