# Reset hidden visibility for symbols of C libs for now here
# TODO: check if librcps should not be linked static anyway, the plugin does not share it with anything installed
set(CMAKE_C_VISIBILITY_PRESET)
# Reset automoc
set(CMAKE_AUTOMOC)
add_subdirectory( LibRCPS )
