# SPDX-FileCopyrightText: 2024 Carl Schwan <carl@carlschwan.eu>
# SPDX-License-Identifier: BSD-3-Clause

find_package(PkgConfig QUIET)
pkg_check_modules(PKG_LibWps QUIET libwps-0.4 IMPORTED_TARGET)

set(LibWps_VERSION ${PKG_LibWps_VERSION})
set(LibWps_PREFIX ${PKG_LibWps_PREFIX})

find_path(LibWps_INCLUDE_DIRS
    NAMES libwps/libwps.h
    HINTS ${PKG_LibWps_INCLUDE_DIRS}
)

find_library(LibWps_LIBRARIES
    NAMES wps-0.4
    HINTS ${PKG_LibWps_LIBRARY_DIRS}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LibWps
    FOUND_VAR LibWps_FOUND
    REQUIRED_VARS LibWps_PREFIX LibWps_INCLUDE_DIRS
    VERSION_VAR LibWps_VERSION
)

if(LibWps_FOUND AND NOT TARGET LibWps::LibWps)
    add_library(LibWps::LibWps UNKNOWN IMPORTED)
    set_target_properties(LibWps::LibWps PROPERTIES
        IMPORTED_LOCATION "${LibWps_LIBRARIES}"
        INTERFACE_COMPILE_OPTIONS "${PKG_LibWps_CFLAGS}"
        INTERFACE_INCLUDE_DIRECTORIES "${LibWps_INCLUDE_DIRS}"
    )
    if (TARGET PkgConfig::PKG_LibWps)
        target_link_libraries(LibWps::LibWps INTERFACE PkgConfig::PKG_LibWps)
    endif()
endif()

mark_as_advanced(LibWps_LIBRARIES LibWps_INCLUDE_DIRS LibWps_VERSION)

include(FeatureSummary)
set_package_properties(LibWps PROPERTIES
    URL http://libwps.sourceforge.net/
    DESCRIPTION "Microsoft Works Document Library"
)
