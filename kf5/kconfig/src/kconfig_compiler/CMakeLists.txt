
remove_definitions(-DQT_NO_KEYWORDS)

set(kconfig_compiler_SRCS kconfig_compiler.cpp)


add_executable(kconfig_compiler ${kconfig_compiler_SRCS})

find_package(Qt5Xml 5.2.0 REQUIRED NO_MODULE)

target_link_libraries(kconfig_compiler  Qt5::Xml)

# "export" this target too so we can use the LOCATION property of the imported target in
# FindKDE4Internal.cmake to get the full path to the installed executable instead of using FIND_PROGRAM(), Alex
install(TARGETS kconfig_compiler EXPORT KConfigTargets ${INSTALL_TARGETS_DEFAULT_ARGS} )
