include_directories(
        ${KOTEXT_INCLUDES}
        ${KOMAIN_INCLUDES}
        ${FLAKE_INCLUDES})

macro_optional_find_package(WPD)
macro_optional_find_package(WPG)
macro_optional_find_package(LibVisio)
macro_log_feature(LIBVISIO_FOUND "LibVisio" "Visio Import Filter Library" "http://www.freedesktop.org/wiki/Software/libvisio" FALSE "" "Required by the Flow visio import filter")

#add_subdirectory( imageexport )
if (WPD_FOUND AND LIBWPG_FOUND AND LIBVISIO_FOUND)
    add_subdirectory( visio )
endif (WPD_FOUND AND LIBWPG_FOUND AND LIBVISIO_FOUND)
