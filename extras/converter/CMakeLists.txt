include_directories(${KOMAIN_INCLUDES})

set(calligraconverter_SRCS calligraconverter.cpp)

add_executable(calligraconverter ${calligraconverter_SRCS})
ecm_mark_nongui_executable(calligraconverter)

target_link_libraries(calligraconverter komain)

install(TARGETS calligraconverter ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

