# SPDX-FileCopyrightText: 2024 Carl Schwan <carl@carlschwan.eu>
# SPDX-License-Identifier: BSD-3-Clause

find_package(PkgConfig QUIET)
pkg_check_modules(PKG_LibWpd QUIET libwpd-0.10 IMPORTED_TARGET)

set(LibWpd_VERSION ${PKG_LibWpd_VERSION})
set(LibWpd_PREFIX ${PKG_LibWpd_PREFIX})

find_path(LibWpd_INCLUDE_DIRS
    NAMES libwpd/libwpd.h
    HINTS ${PKG_LibWpd_INCLUDE_DIRS}
)

find_library(LibWpd_LIBRARIES
    NAMES wpd wpd-0.10
    HINTS ${PKG_LibWpd_LIBRARY_DIRS}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LibWpd
    FOUND_VAR LibWpd_FOUND
    REQUIRED_VARS LibWpd_PREFIX LibWpd_INCLUDE_DIRS
    VERSION_VAR LibWpd_VERSION
)

if(LibWpd_FOUND AND NOT TARGET LibWpd::LibWpd)
    add_library(LibWpd::LibWpd UNKNOWN IMPORTED)
    set_target_properties(LibWpd::LibWpd PROPERTIES
        IMPORTED_LOCATION "${LibWpd_LIBRARIES}"
        INTERFACE_COMPILE_OPTIONS "${PKG_LibWpd_CFLAGS}"
        INTERFACE_INCLUDE_DIRECTORIES "${LibWpd_INCLUDE_DIRS}"
    )
    if (TARGET PkgConfig::PKG_LibWpd)
        target_link_libraries(LibWpd::LibWpd INTERFACE PkgConfig::PKG_LibWpd)
    endif()
endif()

mark_as_advanced(LibWpd_LIBRARIES LibWpd_INCLUDE_DIRS LibWpd_VERSION)

include(FeatureSummary)
set_package_properties(LibWpd PROPERTIES
    URL "http://libwpd.sourceforge.net/"
    DESCRIPTION "WordPerfect Document Library"
)
