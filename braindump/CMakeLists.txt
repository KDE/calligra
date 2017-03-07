project("braindump")

add_definitions(-DTRANSLATION_DOMAIN=\"braindump\")

include_directories ( ${CMAKE_CURRENT_SOURCE_DIR}/braindumpcore)

add_subdirectory(data)
add_subdirectory(src)
add_subdirectory(plugins)
add_subdirectory(braindumpcore)
