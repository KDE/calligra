set(keximigratetest_SRCS keximigratetest.cpp )

add_executable(keximigratetest ${keximigratetest_SRCS})

target_link_libraries(keximigratetest keximigrate kexicore kexiextendedwidgets keximain)
