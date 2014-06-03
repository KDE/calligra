include_directories(
    ${QT_INCLUDES}
)

set(cstrunner_SRCS
    CSTProcessRunner.cpp
    cstrunner.cpp)

kde4_add_executable(cstrunner ${cstrunner_SRCS})

target_link_libraries(cstrunner ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})

install(TARGETS cstrunner ${INSTALL_TARGETS_DEFAULT_ARGS})

