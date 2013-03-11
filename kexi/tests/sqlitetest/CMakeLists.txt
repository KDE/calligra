include_directories( ${CMAKE_SOURCE_DIR}/kexi ${KDE4_INCLUDES}  )

set(kexisqlitetest_SRCS main.cpp)

kde4_add_executable(kexisqlitetest NOGUI ${kexisqlitetest_SRCS})

target_link_libraries(kexisqlitetest ${KDE4_KDEUI_LIBS} kexidb ${CALLIGRADB_LIBS})
