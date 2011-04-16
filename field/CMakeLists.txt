project("field")

include_directories (${KDE4_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR}/fieldcore)

add_subdirectory(data)
add_subdirectory(src)
add_subdirectory(plugins)
add_subdirectory(fieldcore)