

include_directories( ${CMAKE_SOURCE_DIR}/kexi ${KDE4_INCLUDES}  )


########### next target ###############

set(kexidbdrivercombotest_SRCS kexidbdrivercombotest.cpp )


kde4_add_executable(kexidbdrivercombotest ${kexidbdrivercombotest_SRCS})

target_link_libraries(kexidbdrivercombotest  ${KDE4_KDECORE_LIBS} kexidb kexiextendedwidgets )
