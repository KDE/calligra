
project(kivio)

macro_optional_find_package(PythonLibs)
macro_log_feature(EXIV2_FOUND "PythonLibs" "Python Libraries not found" "http://www.python.org" FALSE "" "Required by Kivio for optional functionality")

macro_bool_to_01(PYTHON_INCLUDE_PATH HAVE_PYTHON)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config-kivio.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-kivio.h)

add_subdirectory(src)
add_subdirectory(templates)
