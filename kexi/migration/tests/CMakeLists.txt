set(keximigratetest_SRCS keximigratetest.cpp )

kde4_add_executable(keximigratetest ${keximigratetest_SRCS})

target_link_libraries(keximigratetest  ${KDE4_KDECORE_LIBS} keximigrate kexicore kexidb
                      kexiextendedwidgets keximain )
