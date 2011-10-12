include_directories(
    ${KOMAIN_INCLUDES}
    ${CMAKE_SOURCE_DIR}/tables
    ${CMAKE_SOURCE_DIR}/libs/kopageapp
)

set(calligraconverter_SRCS calligraconverter.cpp )

kde4_add_executable(calligraconverter ${calligraconverter_SRCS})

target_link_libraries(calligraconverter
    komain
    calligratablescommon)

install(TARGETS calligraconverter  ${INSTALL_TARGETS_DEFAULT_ARGS})

