# SPDX-FileCopyrightText: 2024 Carl Schwan <carl@carlschwan.eu>
# SPDX-License-Identifier: BSD-3-Clause

find_package(PkgConfig QUIET)
pkg_check_modules(PKG_LibWpg QUIET libwpg-0.3 IMPORTED_TARGET)

set(LibWpg_VERSION ${PKG_LibWpg_VERSION})
set(LibWpg_PREFIX ${PKG_LibWpg_PREFIX})

find_path(LibWpg_INCLUDE_DIRS
    NAMES libwpg/libwpg.h
    HINTS ${PKG_LibWpg_INCLUDE_DIRS}
)

find_library(LibWpg_LIBRARIES
    NAMES wpg-0.3
    HINTS ${PKG_LibWpg_LIBRARY_DIRS}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LibWpg
    FOUND_VAR LibWpg_FOUND
    REQUIRED_VARS LibWpg_PREFIX LibWpg_INCLUDE_DIRS
    VERSION_VAR LibWpg_VERSION
)

if(LibWpg_FOUND AND NOT TARGET LibWpg::LibWpg)
    add_library(LibWpg::LibWpg UNKNOWN IMPORTED)
    set_target_properties(LibWpg::LibWpg PROPERTIES
        IMPORTED_LOCATION "${LibWpg_LIBRARIES}"
        INTERFACE_COMPILE_OPTIONS "${PKG_LibWpg_CFLAGS}"
        INTERFACE_INCLUDE_DIRECTORIES "${LibWpg_INCLUDE_DIRS}"
    )
    if (TARGET PkgConfig::PKG_LibWpg)
        target_link_libraries(LibWpg::LibWpg INTERFACE PkgConfig::PKG_LibWpg)
    endif()
endif()

mark_as_advanced(LibWpg_LIBRARIES LibWpg_INCLUDE_DIRS LibWpg_VERSION)

include(FeatureSummary)
set_package_properties(LibWpg PROPERTIES
    URL http://libwpg.sourceforge.net/
    DESCRIPTION "WordPerfect Graphics Library"
)
