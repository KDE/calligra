# SPDX-FileCopyrightText: 2024 Carl Schwan <carl@carlschwan.eu>
# SPDX-License-Identifier: BSD-3-Clause

find_package(PkgConfig QUIET)
pkg_check_modules(PKG_LibRevenge QUIET librevenge-0.0 IMPORTED_TARGET)

set(LibRevenge_VERSION ${PKG_LibRevenge_VERSION})
set(LibRevenge_PREFIX ${PKG_LibRevenge_PREFIX})

find_path(LibRevenge_INCLUDE_DIRS
    NAMES librevenge/librevenge.h
    HINTS ${PKG_LibRevenge_INCLUDE_DIRS}
)

find_library(LibRevenge_LIBRARIES
    NAMES revenge librevenge revenge-0.0 librevenge-0.0
    HINTS ${PKG_LibRevenge_LIBRARY_DIRS}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LibRevenge
    FOUND_VAR LibRevenge_FOUND
    REQUIRED_VARS LibRevenge_PREFIX LibRevenge_INCLUDE_DIRS
    VERSION_VAR LibRevenge_VERSION
)

if(LibRevenge_FOUND AND NOT TARGET LibRevenge::LibRevenge)
    add_library(LibRevenge::LibRevenge UNKNOWN IMPORTED)
    set_target_properties(LibRevenge::LibRevenge PROPERTIES
        IMPORTED_LOCATION "${LibRevenge_LIBRARIES}"
        INTERFACE_COMPILE_OPTIONS "${PKG_LibRevenge_CFLAGS}"
        INTERFACE_INCLUDE_DIRECTORIES "${LibRevenge_INCLUDE_DIRS}"
    )
    if (TARGET PkgConfig::PKG_LibRevenge)
        target_link_libraries(LibRevenge::LibRevenge INTERFACE PkgConfig::PKG_LibRevenge)
    endif()
endif()

mark_as_advanced(LibRevenge_LIBRARIES LibRevenge_INCLUDE_DIRS LibRevenge_VERSION)

include(FeatureSummary)
set_package_properties(LibRevenge PROPERTIES
    URL "http://sf.net/p/libwpd/librevenge/"
    DESCRIPTION "A base library for writing document import filters"
)
