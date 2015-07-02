set(kexisqlitetest_SRCS main.cpp)

add_executable(kexisqlitetest NOGUI ${kexisqlitetest_SRCS})

target_link_libraries(kexisqlitetest KDb)
