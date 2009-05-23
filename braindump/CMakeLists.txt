project("braindump")

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules )

find_package(KDE4 REQUIRED)
include(KDE4Defaults)
include(MacroLibrary)
find_package(KOffice REQUIRED)

add_subdirectory(src)
add_subdirectory(plugins)
