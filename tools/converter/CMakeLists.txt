include_directories(
    ${KOMAIN_INCLUDES}
    ${CMAKE_SOURCE_DIR}/tables
    ${CMAKE_SOURCE_DIR}/libs/kopageapp
)

set(koconverter_SRCS koconverter.cpp )

kde4_add_executable(koconverter ${koconverter_SRCS})

target_link_libraries(koconverter
    komain
    calligratablescommon)

install(TARGETS koconverter  ${INSTALL_TARGETS_DEFAULT_ARGS})

