

include_directories( ${CMAKE_SOURCE_DIR}/kexi ${KDE4_INCLUDES}  )


########### next target ###############

set(kexidbdrivercombotest_SRCS kexidbdrivercombotest.cpp )


add_executable(kexidbdrivercombotest ${kexidbdrivercombotest_SRCS})

target_link_libraries(kexidbdrivercombotest   kexidb kexiextendedwidgets )
